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
use MongoDB\Operation\CreateIndexes;
use MongoDB\Operation\DropIndexes;
use MongoDB\Operation\ListIndexes;

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
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Violating If a constraint is violated
     * @throws \Caridea\Dao\Exception\Inoperable If an API is used incorrectly
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    protected function createIndexes(\MongoDB\Driver\Manager $manager, string $db, string $collection, \ConstVector<MongoIndex> $indexes): \ConstVector<string>
    {
        $operation = new CreateIndexes(
            $db, $collection,
            $indexes->map($a ==> $a->toArray())->toArray()
        );
        try {
            $server = $manager->selectServer(new ReadPreference(ReadPreference::RP_PRIMARY));
            return new ImmVector($operation->execute($server));
        } catch (\Exception $e) {
            throw \Caridea\Dao\Exception\Translator\MongoDb::translate($e);
        }
    }

    /**
     * Deletes some indexes in a collection.
     *
     * This method will first check for the existence of the supplied indexes
     * and if found, will drop them.
     *
     * @param $manager - The MongoDB manager
     * @param $db - The database name
     * @param $collection - The collection name
     * @param $indexes - The indexes to delete
     * @return - The names of the created indexes
     * @see https://docs.mongodb.com/manual/reference/command/dropIndexes/
     * @throws \Caridea\Dao\Exception\Unreachable If the connection fails
     * @throws \Caridea\Dao\Exception\Unretrievable If the document doesn't exist
     * @throws \Caridea\Dao\Exception\Violating If a constraint is violated
     * @throws \Caridea\Dao\Exception\Inoperable If an API is used incorrectly
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    protected function dropIndexes(\MongoDB\Driver\Manager $manager, string $db, string $collection, \ConstSet<string> $names): \ConstVector<mixed>
    {
        try {
            $results = Vector{};
            $server = $manager->selectServer(new ReadPreference(ReadPreference::RP_PRIMARY));
            $op = new ListIndexes($db, $collection);
            $delete = Set{};
            foreach ($op->execute($server) as $k => $v) {
                if ($names->contains($v->getName())) {
                    $delete[] = $v->getName();
                }
            }
            if (!$delete->isEmpty()) {
                foreach ($delete as $name) {
                    $operation = new DropIndexes($db, $collection, $name);
                    $results[] = $operation->execute($server);
                }
            }
            return $results->toImmVector();
        } catch (\Exception $e) {
            throw \Caridea\Dao\Exception\Translator\MongoDb::translate($e);
        }
    }
}
