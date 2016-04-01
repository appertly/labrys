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
namespace Labrys\Auth;

/**
 * Class for loading owner objects.
 */
interface OwnerLoader<Tk,+Tv as Owner<Tk>>
{
    /**
     * Tries to find an owner record for the ID given.
     *
     * @param $id - The ID
     * @return The owner record found, or null
     * @throws \Labrys\Db\Exception\System in case of database error
     */
    public function findOwnerById(mixed $id) : ?Tv;

    /**
     * Loads an owner record for the ID given.
     *
     * @param $id - The ID
     * @return The owner record found
     * @throws \Labrys\Db\Exception\Retrieval if the record isn't found
     * @throws \Labrys\Db\Exception\System in case of database error
     */
    public function getOwnerById(mixed $id) : Tv;

    /**
     * Tries to find an owner record for the URL slug given.
     *
     * @param $slug - The URL slug
     * @return The owner record found, or null
     * @throws \Labrys\Db\Exception\System in case of database error
     */
    public function findOwnerBySlug(string $slug) : ?Tv;

    /**
     * Loads an owner record for the ID given.
     *
     * @param $slug - The URL slug
     * @return The owner record found
     * @throws \Labrys\Db\Exception\Retrieval if the record isn't found
     * @throws \Labryl\Db\Exception\System in case of database error
     */
    public function getOwnerBySlug(string $slug) : Tv;
}
