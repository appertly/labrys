<?hh // strict
/**
 * Labrys
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @copyright 2015-2016 Appertly
 * @license   Apache-2.0
 */
namespace Labrys\Db;

use MongoDB\BSON\ObjectID;
use MongoDB\Driver\Cursor;
use MongoDB\Driver\Manager;
use MongoDB\Driver\ReadPreference;
use MongoDB\Driver\WriteConcern;
use MongoDB\Driver\WriteResult;
use Labrys\Getter;

/**
 * Abstract MongoDB DAO Service
 */
abstract class AbstractMongoDao<T> extends \Caridea\Dao\MongoDb implements EntityRepo<T>, DbRefResolver<T>
{
    use MongoHelper;

    /**
     * Whether to enforce optimistic locking
     */
    private bool $versioned = true;
    /**
     * Whether entities will be put in the first-level cache
     */
    private bool $caching = true;
    /**
     * The MongoDB type map when reading records
     */
    private array<string,?string> $typeMap = ['root' => null, 'document' => null];
    /**
     * The MongoDB read preference
     */
    private ?ReadPreference $readPreference;
    /**
     * The MongoDB write concern
     */
    private ?WriteConcern $writeConcern;
    /**
     * First-level cache
     */
    private Map<string,T> $cache = Map{};

    /**
     * Creates a new AbstractMongoService.
     *
     * Current accepted configuration values:
     * * `versioned` – Whether to enforce optimistic locking via a version field (default: true)
     * * `caching` – Whether to cache entities by ID (default: true)
     * * `typeMapRoot` – The type used to unserialize BSON root documents
     * * `typeMapDocument` – The type used to unserialize BSON nested documents
     * * `readPreference` – Must be a `MongoDB\Driver\ReadPreference`
     * * `writeConcern` – Must be a `MongoDB\Driver\WriteConcern`
     *
     * As for the `typeMap` options, you can see
     * [Deserialization from BSON](http://php.net/manual/en/mongodb.persistence.deserialization.php#mongodb.persistence.typemaps)
     * for more information.
     *
     * @param $manager - The MongoDB Manager
     * @param $collection - The collection to wrap
     * @param $validator - Optional validator
     * @param $options - Map of configuration values
     */
    public function __construct(
        Manager $manager,
        string $collection,
        protected ?\Caridea\Validate\Validator $validator = null,
        ?\ConstMap<string,mixed> $options = null,
    ) {
        parent::__construct($manager, $collection);
        if ($options !== null) {
            $this->versioned = $options->containsKey('version') ?
                (bool) $options['version'] : true;
            $this->caching = $options->containsKey('caching') ?
                (bool) $options['caching'] : true;
            if ($options->containsKey('typeMapRoot')) {
                $r = $options['typeMapRoot'];
                $this->typeMap['root'] = $r === null ? null : (string)$r;
            }
            if ($options->containsKey('typeMapDocument')) {
                $d = $options['typeMapDocument'];
                $this->typeMap['document'] = $d === null ? null : (string)$d;
            }
            $rp = $options['readPreference'] ?? null;
            if ($rp instanceof ReadPreference) {
                $this->readPreference = $rp;
            }
            $wc = $options['writeConcern'] ?? null;
            if ($wc instanceof WriteConcern) {
                $this->writeConcern = $wc;
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public function findOne(\ConstMap<string,mixed> $criteria) : ?T
    {
        return $this->maybeCache(
            $this->doExecute(function (Manager $m, string $c) use ($criteria) {
                $q = new \MongoDB\Driver\Query($criteria->toArray(), ['limit' => 1]);
                $res = $m->executeQuery($c, $q, $this->readPreference);
                $res->setTypeMap($this->typeMap);
                $resa = $res->toArray();
                return count($resa) > 0 ? current($resa) : null;
            })
        );
    }

    /**
     * {@inheritDoc}
     */
    public function findAll(\ConstMap<string,mixed> $criteria, ?\Caridea\Http\Pagination $pagination = null) : Cursor<T>
    {
        /* HH_IGNORE_ERROR[4101]: Cursor will return whatever the user specifies in the typeMap */
        return $this->doExecute(function (Manager $m, string $c) use ($criteria, $pagination) {
            $qo = [];
            if ($pagination) {
                if ($pagination->getMax() != PHP_INT_MAX) {
                    $qo['limit'] = $pagination->getMax();
                }
                $qo['skip'] = $pagination->getOffset();
                $sorts = [];
                foreach ($pagination->getOrder() as $k => $v) {
                    $sorts[$k] = $v ? 1 : -1;
                }
                if (count($sorts) > 0) {
                    $qo['sort'] = $sorts;
                }
            }
            $q = new \MongoDB\Driver\Query($criteria->toArray(), $qo);
            $res = $m->executeQuery($c, $q, $this->readPreference);
            $res->setTypeMap($this->typeMap);
            return $res;
        });
    }

    /**
     * {@inheritDoc}
     */
    public function findById(mixed $id) : ?T
    {
        try {
            $mid = $this->toId($id);
        } catch (\MongoDB\Driver\Exception\InvalidArgumentException $e) {
            if ($e->getMessage() === 'Invalid BSON ID provided') {
                throw new \Caridea\Dao\Exception\Unretrievable('Could not find document', 0, $e);
            }
            throw $e;
        }
        return $this->getFromCache((string)$id) ??
            $this->findOne(ImmMap{'_id' => $mid});
    }

    /**
     * {@inheritDoc}
     */
    public function get(mixed $id) : T
    {
        return $this->ensure($id, $this->findById($id));
    }

    /**
     * {@inheritDoc}
     */
    public function getAll(\ConstVector<mixed> $ids) : Traversable<T>
    {
        if ($ids->isEmpty()) {
            return ImmVector{};
        }
        try {
            $mids = $this->toIds($ids);
        } catch (\MongoDB\Driver\Exception\InvalidArgumentException $e) {
            if ($e->getMessage() === 'Invalid BSON ID provided') {
                throw new \Caridea\Dao\Exception\Unretrievable('Could not load documents', 0, $e);
            }
            throw $e;
        }
        $cmp = array_flip($ids->map($a ==> (string) $a));
        $fromCache = new ImmVector(array_intersect_key($this->cache, $cmp));
        if (count($fromCache) === count($ids)) {
            return $fromCache;
        } elseif (count($fromCache) > 0) {
            $mids = $mids->filter($a ==> !array_key_exists((string)$a, $fromCache));
            return $fromCache->concat(
                $this->maybeCacheAll($this->findAll(ImmMap{'_id' => ['$in' => $mids->toArray()]}))
            );
        } else {
            return $this->maybeCacheAll($this->findAll(ImmMap{'_id' => ['$in' => $mids->toArray()]}));
        }
    }

    /**
     * {@inheritDoc}
     */
    public function getInstanceMap(Traversable<T> $entities) : ImmMap<string,T>
    {
        $instances = Map{};
        foreach ($entities as $entity) {
            $instances[(string)Getter::getId($entity)] = $entity;
        }
        return $instances->toImmMap();
    }

    /**
     * {@inheritDoc}
     */
    public function isResolvable(string $ref): bool
    {
        return strstr($this->collection, '.') === ".$ref";
    }

    /**
     * {@inheritDoc}
     */
    public function resolve(shape('$ref' => string, '$id' => mixed) $ref): ?T
    {
        if (!$this->isResolvable($ref['$ref'])) {
            throw new \InvalidArgumentException("Unsupported reference type: " . $ref['$ref']);
        }
        return $this->findById($ref['$id']);
    }

    /**
     * {@inheritDoc}
     */
    public function resolveAll(Traversable<shape('$ref' => string, '$id' => mixed)> $refs): Traversable<T>
    {
        $types = Set{};
        $ids = Vector{};
        foreach ($refs as $ref) {
            $types[] = $ref['$ref'];
            $ids[] = $ref['$id'];
        }
        foreach ($types as $type) {
            if (!$this->isResolvable($type)) {
                throw new \InvalidArgumentException("Unsupported reference type: " . $type);
            }
        }
        return $this->getAll($ids);
    }

    /**
     * Creates a record.
     *
     * @param $record - The record to insert, ready to go
     * @return - Whatever MongoDB returns
     * @throws \Caridea\Validate\Exception\Invalid if validation fails
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Violating If a constraint is violated
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    protected function doCreate(\ConstMap<string,mixed> $record): WriteResult
    {
        $record = $record->toArray();

        // check validation
        if ($this->validator) {
            $this->validator->assert($record);
        }
        if ($this->versioned) {
            $record['version'] = 0;
        }

        return $this->doExecute(function (Manager $m, string $c) use ($record) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->insert($record);
            return $m->executeBulkWrite($c, $bulk, $this->writeConcern);
        });
    }

    /**
     * Creates a record using a MongoDB `Persistable`.
     *
     * @param $record - The document to insert, ready to go
     * @return - Whatever MongoDB returns
     * @throws \Caridea\Validate\Exception\Invalid if validation fails
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Violating If a constraint is violated
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     * @since 0.3.0
     */
    protected function doPersist(\MongoDB\BSON\Persistable $record): WriteResult
    {
        // check validation
        if ($this->validator) {
            $this->validator->assert($record->bsonSerialize());
        }
        return $this->doExecute(function (Manager $m, string $c) use ($record) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->insert($record);
            return $m->executeBulkWrite($c, $bulk, $this->writeConcern);
        });
    }

    /**
     * Updates a record.
     *
     * You should check optimistic lock version *before* calling this method.
     *
     * @param $entity - The entity to update
     * @return - Whatever MongoDB returns
     * @throws \Caridea\Validate\Exception\Invalid if validation fails
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Violating If a constraint is violated
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     * @since 0.5.1
     */
    protected function doUpdateModifiable(Entity\Modifiable $entity): ?WriteResult
    {
        if (!$entity->isDirty()) {
            return null;
        }
        $mid = Getter::getId($entity);
        // check validation
        if ($this->validator) {
            $this->validator->assert($entity->bsonSerialize());
        }
        $ops = $entity->getChanges()->map($a ==> $a->toArray())->toArray();
        if ($this->versioned) {
            $ops['$inc']['version'] = 1;
        }
        $this->cache->removeKey((string)$mid);
        return $this->doExecute(function (Manager $m, string $c) use ($mid, $ops) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->update(['_id' => $mid], $ops);
            return $m->executeBulkWrite($c, $bulk, $this->writeConcern);
        });
    }

    /**
     * Updates a record.
     *
     * @param \MongoDB\BSON\ObjectID|string $id The document identifier, either a string or `ObjectID`
     * @param $record - The record to update, ready to go
     * @return - Whatever MongoDB returns
     * @throws \Caridea\Validate\Exception\Invalid if validation fails
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Conflicting If optimistic/pessimistic lock fails
     * @throws \Caridea\Dao\Exception\Violating If a constraint is violated
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    protected function doUpdate(mixed $id, \ConstMap<string,mixed> $record, ?\ConstMap<string,mixed> $extraOps = null): WriteResult
    {
        $record = $record->toArray();

        // ensure record exists
        $mid = $this->toId($id);
        $orig = $this->get($id);

        $ops = [];
        if ($extraOps) {
            foreach ($extraOps as $k => $v) {
                $ops[$k] = $v;
            }
        }

        // check optimistic locking
        if ($this->versioned) {
            if (array_key_exists('version', $record)) {
                $origVersion = (int)Getter::get($orig, 'version');
                if ($origVersion > $record['version']) {
                    throw new \Caridea\Dao\Exception\Conflicting("Document version conflict");
                }
                unset($record['version']);
            }
            $ops['$inc'] = ['version' => 1];
        }
        if (count($record) > 0) {
            $ops['$set'] = $record;
        }

        // check validation
        if ($this->validator) {
            if ($orig instanceof KeyedContainer) {
                $this->validator->assert(array_merge($orig, $record));
            } elseif ($orig instanceof \MongoDB\BSON\Persistable) {
                $this->validator->assert(array_merge($orig->bsonSerialize(), $record));
            }
        }

        // do update operation
        $this->cache->removeKey((string)$id);
        return $this->doExecute(function (Manager $m, string $c) use ($mid, $ops) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->update(['_id' => $mid], $ops);
            return $m->executeBulkWrite($c, $bulk, $this->writeConcern);
        });
    }

    /**
     * Deletes a record.
     *
     * @param \MongoDB\BSON\ObjectID|string $id The document identifier, either a string or `ObjectID`
     * @return - Whatever MongoDB returns
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    protected function doDelete(mixed $id): WriteResult
    {
        $mid = $this->toId($id);
        $this->cache->removeKey((string)$id);
        return $this->doExecute(function (Manager $m, string $c) use ($mid) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->delete(['_id' => $mid], ['limit' => 1]);
            return $m->executeBulkWrite($c, $bulk, $this->writeConcern);
        });
    }

    /**
     * Possibly add the entity to the cache.
     *
     * @param $entity - The entity to possibly cache
     * @return - The same entity that came in
     */
    protected function maybeCache(?T $entity) : ?T
    {
        if ($this->caching && $entity !== null) {
            $id = (string)Getter::getId($entity);
            if (!$this->cache->containsKey($id)) {
                $this->cache[$id] = $entity;
            }
        }
        return $entity;
    }

    /**
     * Possibly add entities to the cache.
     *
     * @param $entities - The entities to possibly cache
     * @return - The same entities that came in
     */
    protected function maybeCacheAll(Cursor<T> $entities) : Traversable<T>
    {
        if ($this->caching) {
            $results = $entities->toArray();
            foreach ($results as $entity) {
                 if ($entity !== null) {
                    $id = (string)Getter::getId($entity);
                    if (!$this->cache->containsKey($id)) {
                        $this->cache[$id] = $entity;
                    }
                }
            }
            return $results;
        } else {
            return $entities;
        }
    }

    /**
     * Gets an entry from the cache
     *
     * @param $id - The cache key
     * @return - The entity found or null
     */
    protected function getFromCache(string $id) : ?T
    {
        return $this->cache[$id] ?? null;
    }
}
