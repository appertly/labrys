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
namespace Labrys\Auth;

/**
 * Represents a group that can contain multiple users.
 */
interface GroupRepo<T>
{
    /**
     * Gets an account by its slug
     *
     * @param $slug - The account slug
     * @return - The account found or `null`
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function findBySlug(string $slug): ?Group<T>;

    /**
     * Gets an account by its slug
     *
     * @param $slug - The account slug
     * @return - The account found
     * @throws \Caridea\Dao\Exception\Unretrievable If the entity doesn't exist
     * @throws \Caridea\Dao\Exception\Generic If any other database problem occurs
     */
    public function getBySlug(string $slug): Group<T>;
}
