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
 * A wrapper around a `Traversable` that includes the total count of the superset.
 */
class CursorSubset<T> extends \IteratorIterator<T> implements \JsonSerializable
{
    private int $total;

    /**
     * Create a new CursorSubset.
     *
     * @param $iterable - The traversable to wrap
     * @param $total - The total number of items in the superset
     * @throws \RangeException if the total is negative
     */
    public function __construct(\Traversable<T> $iterable, int $total)
    {
        parent::__construct($iterable);
        if ($total < 0) {
            throw new \RangeException("Total cannot be a negative number");
        }
        $this->total = $total;
    }

    /**
     * Gets the superset total.
     *
     * @return - The total number of items in the superset (never negative).
     */
    public function getTotal(): int
    {
        return $this->total;
    }

    /**
     * Return data which can be serialized with json_encode.
     */
    public function jsonSerialize(): mixed
    {
        $it = $this->getInnerIterator();
        return $it instanceof \JsonSerializable ? $it : $this->toArray();
    }

    /**
     * Converts this thing into an array.
     *
     * @return - The array version of this thing
     */
    public function toArray(): array<T>
    {
        $it = $this->getInnerIterator();
        if ($it instanceof \MongoDB\Driver\Cursor) {
            return $it->toArray();
        } elseif ($it instanceof Iterable) {
            return $it->toArray();
        } elseif ($it instanceof \ArrayObject) {
            return $it->getArrayCopy();
        } elseif ($it instanceof \ArrayIterator) {
            return $it->getArrayCopy();
        }
        return iterator_to_array($it, false);
    }
}
