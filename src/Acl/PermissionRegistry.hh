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
namespace Labrys\Acl;

/**
 * Maintains a list of entity permissions.
 */
interface PermissionRegistry
{
    /**
     * Gets the list of permissions.
     *
     * The keys in this `Map` correspond to ACL types. The values should be a
     * `Map`, its keys should be the machine name for a permission (e.g.
     * `write`), and its values a human-readable description.
     *
     * ```
     * ImmMap{
     *     'core\\account' => ImmMap {
     *         'reticulate-splines' => 'Reticulate any Spline in the account'
     *     }
     * }
     * ```
     *
     * The idea is that a UI could collect the permissions and allow user-
     * configurable permissions per role.
     *
     * @return - The list of permissions
     */
    public function getPermissions(): \ConstMap<string,\ConstMap<string,string>>;
}
