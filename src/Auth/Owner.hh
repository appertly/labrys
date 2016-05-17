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
 * Represents an owner of records. Maybe a user, maybe an organization.
 */
interface Owner<T>
{
    /**
     * Gets the entity ID.
     *
     * @return - The entity's ID
     */
    public function getId(): T;

    /**
     * Gets the entity's display name.
     *
     * @return - The entity's display name
     */
    public function getDisplayName(): string;

    /**
     * Gets the URL slug for this entity.
     *
     * @return - The URL slug
     */
    public function getSlug(): string;

    /**
     * Gets the TimeZone for this entity.
     *
     * If the entity belongs to a parent record, e.g. a user belonging to a
     * group, the parent value should be returned if none is available on this
     * entity.
     *
     * Should default to system time zone if none is available.
     *
     * @return - The owner's time zone
     */
    public function getTimeZone(): \DateTimeZone;

    /**
     * Gets whether this group is disabled.
     *
     * @return - Whether the group is disabled
     */
    public function isDisabled(): bool;

    /**
     * Converts this entity to an array.
     *
     * @return - The array version
     */
    public function toArray(): array<string,mixed>;
}
