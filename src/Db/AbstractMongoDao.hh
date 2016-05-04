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
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */
namespace Labrys\Db;

use MongoDB\BSON\ObjectID;
use MongoDB\Driver\Cursor;
use MongoDB\Driver\Manager;
use MongoDB\Driver\WriteResult;

/**
 * Abstract MongoDB DAO Service
 */
abstract class AbstractMongoDao<T> implements EntityRepo<T>
{
    use MongoHelper;

    /**
     * @var Whether to enforce optimistic locking
     */
    private bool $versioned = true;
    /**
     * @var Whether entities will be put in the first-level cache
     */
    private bool $caching = true;
    /**
     * @var The MongoDB type map when reading records
     */
    private array<string,?string> $typeMap = ['root' => null, 'document' => null];
    /**
     * @var First-level cache
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
        private Manager $manager,
        private string $collection,
        protected ?\Caridea\Validate\Validator $validator = null,
        ?\ConstMap<string,mixed> $options = null
    )
    {
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
        }
    }

    /**
     * Finds a single record by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @return The object found or null if none
     */
    public function findOne(\ConstMap<string,mixed> $criteria) : ?T
    {
        return $this->maybeCache(
            $this->doExecute(function (Manager $m, string $c) use ($criteria) {
                $q = new \MongoDB\Driver\Query($criteria->toArray(), ['limit' => 1]);
                $res = $m->executeQuery($c, $q);
                $res->setTypeMap($this->typeMap);
                $resa = $res->toArray();
                return count($resa) > 0 ? current($resa) : null;
            })
        );
    }

    /**
     * Finds several records by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @param $pagination - Optional pagination parameters
     * @return The objects found or null if none
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
            $res = $m->executeQuery($c, $q);
            $res->setTypeMap($this->typeMap);
            return $res;
        });
    }

    /**
     * Gets a single document by ID.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return The BSON document
     */
    public function findById(mixed $id) : ?T
    {
        return $this->getFromCache((string)$id) ??
            $this->findOne(ImmMap{'_id' => $this->toId($id)});
    }

    /**
     * Gets a single document by ID, throwing an exception if it's not found.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return The BSON document
     * @throws \Labrys|Db\Exception\Retrieval If the document doesn't exist
     */
    public function get(mixed $id) : T
    {
        return $this->ensure($id, $this->findById($id));
    }

    /**
     * Gets several documents by ID.
     *
     * @param $ids - Array of either strings or `ObjectID`s.
     * @return The results
     */
    public function getAll(\ConstVector<mixed> $ids) : Traversable<T>
    {
        if ($ids->isEmpty()) {
            return ImmVector{};
        }
        $mids = $ids->map(
            $a ==> $a instanceof ObjectID ? $a : new ObjectID((string) $a)
        );
        return $this->maybeCacheAll($this->findAll(ImmMap{'_id' => ['$in' => $mids->toArray()]}));
    }

    /**
     * Gets a Map that relates identifier to instance
     *
     * @param $entities - The entities to "zip"
     * @return The instances keyed by identifier
     */
    public function getInstanceMap(Traversable<T> $entities) : ImmMap<string,T>
    {
        $instances = Map{};
        foreach ($entities as $entity) {
            $instances[(string)$this->getId($entity)] = $entity;
        }
        return $instances->toImmMap();
    }

    /**
     * Creates a record.
     *
     * @param $record - The record to insert, ready to go
     * @return Whatever MongoDB returns
     * @throws \Caridea\Validate\Exception\Invalid if validation fails
     * @throws \Labrys|Db\Exception\Integrity If a unique constraint is violated
     * @throws \Labrys|Db\Exception\System If any other database problem occurs
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
            return $m->executeBulkWrite($c, $bulk);
        });
    }

    /**
     * Updates a record.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @param $record - The record to update, ready to go
     * @return Whatever MongoDB returns
     * @throws \Caridea\Validate\Exception\Invalid if validation fails
     * @throws \Labrys|Db\Exception\Retrieval If the document doesn't exist
     * @throws \Labrys|Db\Exception\Integrity If a unique constraint is violated
     * @throws \Labrys|Db\Exception\Concurrency If optimistic locking fails
     * @throws \Labrys|Db\Exception\System If any other database problem occurs
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
                if (is_array($orig) && $orig['version'] > $record['version']) {
                    throw new Exception\Concurrency("Document version conflict");
                }
                unset($record['version']);
            }
            $ops['$inc'] = ['version' => 1];
        }
        $ops['$set'] = $record;

        // check validation
        if ($this->validator) {
            $this->validator->assert(array_merge($orig, $record));
        }

        // do update operation
        $this->cache->removeKey((string)$id);
        return $this->doExecute(function (Manager $m, string $c) use ($mid, $ops) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->update(['_id' => $mid], $ops);
            return $m->executeBulkWrite($c, $bulk);
        });
    }

    /**
     * Deletes a record.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return Whatever MongoDB returns
     * @throws \Labrys|Db\Exception If a database problem occurs
     */
    protected function doDelete(mixed $id): WriteResult
    {
        $mid = $this->toId($id);
        $this->cache->removeKey((string)$id);
        return $this->doExecute(function (Manager $m, string $c) use ($mid) {
            $bulk = new \MongoDB\Driver\BulkWrite();
            $bulk->delete(['_id' => $mid], ['limit' => 1]);
            return $m->executeBulkWrite($c, $bulk);
        });
    }

    /**
     * Executes something in the context of the collection.
     *
     * Exceptions are caught and translated.
     *
     * @param $cb - The closure to execute, takes the entityManager
     * @return Whatever the function returns, this method also returns
     * @throws \Labrys|Db\Exception If a database problem occurs
     */
    protected function doExecute<Ta>((function(Manager, string): Ta) $cb) : Ta
    {
        try {
            return $cb($this->manager, $this->collection);
        } catch (\Exception $e) {
            throw self::translateException($e);
        }
    }

    /**
     * Possibly add the entity to the cache.
     *
     * @param $entity - The entity to possibly cache
     * @return The same entity that came in
     */
    protected function maybeCache(?T $entity) : ?T
    {
        if ($this->caching && $entity !== null) {
            $id = (string)$this->getId($entity);
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
     * @return The same entities that came in
     */
    protected function maybeCacheAll(Cursor<T> $entities) : Traversable<T>
    {
        if ($this->caching) {
            $results = $entities->toArray();
            foreach ($results as $entity) {
                 if ($entity !== null) {
                    $id = (string)$this->getId($entity);
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
     * Extracts the ID from a thing
     */
    private function getId(mixed $a) : mixed
    {
        if (is_array($a)) {
            return $a['_id'];
        } elseif ($a instanceof \ConstMap) {
            return $a->get('_id');
        } elseif (is_object($a)) {
            /* HH_IGNORE_ERROR[4062]: This is fine */
            return $a->id;
        }
        return null;
    }

    /**
     * Gets an entry from the cache
     *
     * @param $id - The cache key
     * @return The entity found or null
     */
    protected function getFromCache(string $id) : ?T
    {
        return $this->cache[$id] ?? null;
    }
}
