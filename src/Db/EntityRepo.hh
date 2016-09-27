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

/**
 * Basic interface for entity services.
 */
interface EntityRepo<T>
{
    /**
     * Gets the type of entity produced, mainly for ACL reasons.
     *
     * @return - The entity type
     */
    public function getType(): string;

    /**
     * Gets a Map that relates identifier to instance
     *
     * @param $entities - The entities to "zip"
     * @return - The instances keyed by identifier
     */
    public function getInstanceMap(Traversable<T> $entities): ImmMap<string,T>;

    /**
     * Finds a single record by some arbitrary criteria.
     *
     * @param $criteria Field to value pairs
     * @return - The object found or null if none
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the result cannot be returned
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function findOne(\ConstMap<string,mixed> $criteria): ?T;

    /**
     * Counts several records by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @return - The count of the documents
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the result cannot be returned
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     * @since 0.6.0
     */
    public function countAll(\ConstMap<string,mixed> $criteria): int;

    /**
     * Finds several records by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @param $pagination - Optional pagination parameters
     * @param $totalCount - Return a `CursorSubset` that includes the total
     *        number of records. This is only done if `$pagination` is not using
     *        the defaults.
     * @return - The objects found or null if none
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the result cannot be returned
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function findAll(\ConstMap<string,mixed> $criteria, ?\Caridea\Http\Pagination $pagination = null, ?bool $totalCount = false): Traversable<T>;

    /**
     * Gets a single document by ID.
     *
     * @param $id - The document identifier
     * @return - The entity
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the result cannot be returned
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function findById(mixed $id): ?T;

    /**
     * Gets a single document by ID, throwing an exception if it's not found.
     *
     * @param $id - The document identifier
     * @return - The entity
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function get(mixed $id): T;

    /**
     * Gets several documents by ID.
     *
     * @param $ids - Array of identifiers
     * @return - The results
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the result cannot be returned
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function getAll(\ConstVector<mixed> $ids): Traversable<T>;
}
