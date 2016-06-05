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
namespace Labrys\Db\Entity;

use MongoDB\BSON\ObjectID;

/**
 * Trait for entities with an `ObjectID`
 *
 * @since 0.3.0
 */
trait Identified
{
    /**
     * The document identifier
     */
    protected ObjectID $id;

    /**
     * Gets the document identifier
     *
     * @return - The document ID
     */
    public function getId(): ObjectID
    {
        return $this->id;
    }
}
