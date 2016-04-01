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

/**
 * Abstract DAO-backed Service
 */
abstract class AbstractDaoService<Ta,Tb as EntityRepo<Ta>> implements EntityRepo<Ta>
{
    /**
     * Creates a new AbstractDaoService.
     *
     * @param $dao - The backing dao
     * @param $gatekeeper - The security gatekeeper
     */
    public function __construct(
        protected Tb $dao,
        protected \Labrys\Acl\Gatekeeper $gatekeeper)
    {
    }

    /**
     * Gets the type of entity produced, mainly for ACL reasons.
     *
     * @return The entity type
     */
    public function getType() : string
    {
        return $this->dao->getType();
    }

    /**
     * Finds a single record by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @return The object found or null if none
     */
    public function findOne(\ConstMap<string,mixed> $criteria) : ?Ta
    {
        return $this->dao->findOne($criteria);
    }

    /**
     * Finds several records by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @param $pagination - Optional pagination parameters
     * @return The objects found or null if none
     */
    public function findAll(\ConstMap<string,mixed> $criteria, ?\Caridea\Http\Pagination $pagination = null) : Traversable<Ta>
    {
        return $this->dao->findAll($criteria, $pagination);
    }

    /**
     * Gets a single document by ID.
     *
     * @param $id - The document identifier
     * @return The BSON document
     */
    public function findById(mixed $id) : ?Ta
    {
        $entity = $this->dao->findById($id);
        if ($entity != null) {
            $this->gatekeeper->assert('read', $this->dao->getType(), $id);
        }
        return $entity;
    }

    /**
     * Gets a single document by ID, throwing an exception if it's not found.
     *
     * @param $id - The document identifier
     * @return The entity
     * @throws \Labrys|Db\Exception\Retrieval If the document doesn't exist
     */
    public function get(mixed $id) : Ta
    {
        $this->gatekeeper->assert('read', $this->dao->getType(), (string) $id);
        return $this->dao->get($id);
    }

    /**
     * Gets several documents by ID.
     *
     * @param $ids - Array of identifiers
     * @return The results
     */
    public function getAll(\ConstVector<mixed> $ids) : Traversable<Ta>
    {
        $all = $this->dao->getAll($ids);
        $type = $this->dao->getType();
        foreach ($all as $v) {
            /* HH_FIXME[4005]: This should be a little more fault tolerant */
            $this->gatekeeper->assert('read', $type, $v['_id']);
        }
        return $all;
    }

    /**
     * Gets a Map that relates identifier to instance
     *
     * @param $entities - The entities to "zip"
     * @return The instances keyed by identifier
     */
    public function getInstanceMap(Traversable<Ta> $entities) : ImmMap<string,Ta>
    {
        return $this->dao->getInstanceMap($entities);
    }

    /**
     * Gets the record, but tests the 'write' permission
     *
     * @param $id - The entity id
     */
    protected function getForUpdate(mixed $id) : Ta
    {
        $this->gatekeeper->assert('write', $this->dao->getType(), (string)$id);
        return $this->dao->get($id);
    }
}
