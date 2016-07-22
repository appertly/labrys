<?hh
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

use MongoDB\Driver\ReadPreference;

/**
 * Creates indexes or anything like that at deploy time.
 *
 * Requires the `mongodb/mongodb` composer package to be installed.
 */
trait MongoIndexHelper
{
    use MongoHelper;

    /**
     * Creates some indexes in a collection.
     *
     * @param $manager - The MongoDB manager
     * @param $db - The database name
     * @param $collection - The collection name
     * @param $indexes - The indexes to create
     * @return - The names of the created indexes
     * @see https://docs.mongodb.com/manual/reference/command/createIndexes/
     * @throws \Labrys\Db\Exception\System If a database problem occurs
     */
    protected function createIndexes(\MongoDB\Driver\Manager $manager, string $db, string $collection, \ConstVector<MongoIndex> $indexes): \ConstVector<string>
    {
        $operation = new \MongoDB\Operation\CreateIndexes(
            $db, $collection,
            $indexes->map($a ==> $a->toArray())->toArray()
        );
        try {
            $server = $manager->selectServer(new ReadPreference(ReadPreference::RP_PRIMARY));
            return new ImmVector($operation->execute($server));
        } catch (\Exception $e) {
            throw self::translateException($e);
        }
    }
}
