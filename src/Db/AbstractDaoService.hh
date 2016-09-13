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

use Labrys\Getter;

/**
 * Abstract DAO-backed Service
 */
abstract class AbstractDaoService<Ta> implements EntityRepo<Ta>
{
    /**
     * Creates a new AbstractDaoService.
     *
     * @param $gatekeeper - The security gatekeeper
     */
    public function __construct(protected \Labrys\Acl\Gatekeeper $gatekeeper)
    {
    }

    /**
     * Gets the DAO.
     *
     * @return - The backing DAO
     */
    protected abstract function getDao(): EntityRepo<Ta>;

    /**
     * {@inheritDoc}
     */
    public function getType(): string
    {
        return $this->getDao()->getType();
    }

    /**
     * {@inheritDoc}
     */
    public function countAll(\ConstMap<string,mixed> $criteria): int
    {
        return $this->getDao()->countAll($criteria);
    }

    /**
     * {@inheritDoc}
     */
    public function findOne(\ConstMap<string,mixed> $criteria): ?Ta
    {
        return $this->getDao()->findOne($criteria);
    }

    /**
     * {@inheritDoc}
     */
    public function findAll(\ConstMap<string,mixed> $criteria, ?\Caridea\Http\Pagination $pagination = null): Traversable<Ta>
    {
        return $this->getDao()->findAll($criteria, $pagination);
    }

    /**
     * {@inheritDoc}
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     */
    public function findById(mixed $id): ?Ta
    {
        $dao = $this->getDao();
        $entity = $dao->findById($id);
        if ($entity !== null) {
            $this->gatekeeper->assert('read', $dao->getType(), $id);
        }
        return $entity;
    }

    /**
     * {@inheritDoc}
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     */
    public function get(mixed $id): Ta
    {
        return $this->getAndAssert($id, 'read');
    }

    /**
     * {@inheritDoc}
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     */
    public function getAll(\ConstVector<mixed> $ids): Traversable<Ta>
    {
        $dao = $this->getDao();
        $all = $dao->getAll($ids);
        $this->gatekeeper->assertAll('read', $dao->getType(), $ids);
        return $all;
    }

    /**
     * {@inheritDoc}
     */
    public function getInstanceMap(Traversable<Ta> $entities): ImmMap<string,Ta>
    {
        return $this->getDao()->getInstanceMap($entities);
    }

    /**
     * Gets the entity and asserts an ACL permission.
     *
     * @since 0.5.1
     * @param $id - The entity id
     * @param $verb - The verb (e.g. 'read', 'write')
     * @return - The entity
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     */
    protected function getAndAssert(mixed $id, string $verb): Ta
    {
        $dao = $this->getDao();
        $entity = $dao->get($id);
        $this->gatekeeper->assert($verb, $dao->getType(), $id);
        return $entity;
    }

    /**
     * Gets the record, but tests the 'write' permission
     *
     * @param $id - The entity id
     * @return - The entity
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     * @deprecated 0.5.1:1.0.0 Use `getAndAssert($id, 'write')` instead
     */
    <<__Deprecated("Use getAndAssert(id, 'write') instead")>>
    protected function getForUpdate(mixed $id): Ta
    {
        return $this->getAndAssert($id, 'write');
    }
}
