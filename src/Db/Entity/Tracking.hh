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

/**
 * A trait for entities which can track changes.
 *
 * @since 0.5.1
 */
trait Tracking
{
    require implements Modifiable;

    /**
     * Changes to persist.
     */
    protected Map<string,Map<string,mixed>> $changes = Map{};

    /**
     * Gets the pending changes.
     *
     * @return - The pending changes
     */
    public function getChanges(): \ConstMap<string,Map<string,mixed>>
    {
        return $this->changes;
    }

    /**
     * Whether the object has any changes.
     *
     * @return - Whether the object has any changes
     */
    public function isDirty(): bool
    {
        return !$this->changes->isEmpty();
    }

    /**
     * Sets a field for update.
     *
     * @param $field - The field name
     * @param $value - The field value
     * @return - provides a fluent interface
     */
    protected function fieldSet(string $field, mixed $value): this
    {
        if (!$this->changes->containsKey('$set')) {
            $this->changes['$set'] = Map{};
        }
        $this->changes['$set'][$field] = $value;
        return $this;
    }

    /**
     * Sets a field for removal.
     *
     * @param $field - The field name
     * @return - provides a fluent interface
     */
    protected function fieldUnset(string $field): this
    {
        if (!$this->changes->containsKey('$unset')) {
            $this->changes['$unset'] = Map{};
        }
        $this->changes['$unset'][$field] = '';
        return $this;
    }

    /**
     * Sets a field for increment.
     *
     * @param $field - The field name
     * @param $value - Optional. The increment value. Default is `1`.
     * @return - provides a fluent interface
     */
    protected function fieldIncrement(string $field, int $value = 1): this
    {
        if (!$this->changes->containsKey('$inc')) {
            $this->changes['$inc'] = Map{};
        }
        $this->changes['$inc'][$field] = $value;
        return $this;
    }

    /**
     * Sets a field to the current date.
     *
     * @param $field - The field name
     * @return - provides a fluent interface
     */
    protected function fieldNow(string $field): this
    {
        if (!$this->changes->containsKey('$currentDate')) {
            $this->changes['$currentDate'] = Map{};
        }
        $this->changes['$currentDate'][$field] = 'date';
        return $this;
    }

    /**
     * Pushes a value onto a field.
     *
     * @param $field - The field name
     * @param $value - The value to push
     * @return - provides a fluent interface
     */
    protected function fieldPush(string $field, mixed $value): this
    {
        if (!$this->changes->containsKey('$push')) {
            $this->changes['$push'] = Map{};
        }
        $this->changes['$push'][$field] = $value;
        return $this;
    }

    /**
     * Pushes a value onto an array field.
     *
     * @param $field - The field name
     * @param $value - The values to push
     * @return - provides a fluent interface
     */
    protected function fieldPushAll(string $field, \ConstVector<mixed> $value): this
    {
        if (!$this->changes->containsKey('$push')) {
            $this->changes['$push'] = Map{};
        }
        $this->changes['$push'][$field] = ['$each' => $value->toArray()];
        return $this;
    }

    /**
     * Pulls a value from a array field.
     *
     * In addition to a single value, you can also specify a query document.
     * ```
     * $this->fieldPull('vegetables', 'carrot');
     * $this->fieldPull('listOfDocs', ['foo' => 'bar']);
     * ```
     *
     * @param $field - The field name
     * @param $value - The value to pull
     * @return - provides a fluent interface
     */
    protected function fieldPull(string $field, mixed $value): this
    {
        if (!$this->changes->containsKey('$pull')) {
            $this->changes['$pull'] = Map{};
        }
        $this->changes['$pull'][$field] = $value;
        return $this;
    }
}
