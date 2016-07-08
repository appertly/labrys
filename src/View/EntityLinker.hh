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
namespace Labrys\View;

/**
 * Allows the creation of `<a>` nodes that point to an entity's canonical location.
 */
interface EntityLinker
{
    /**
     * Whether this linker supports the provided entity.
     *
     * @param $entity - The entity to link
     * @return - `true` if the entity is supported
     */
    public function supports(mixed $entity): bool;

    /**
     * Composes the link for a given entity.
     *
     * @param $entity - The entity to link
     * @return - The XHP node for the entity's link
     * @throws \InvalidArgumentException if the entity isn't supported
     */
    public function compose(mixed $entity) : Awaitable<\XHPRoot>;
}
