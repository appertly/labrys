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
 * A MongoDB Index definition
 */
class MongoIndex
{
    private ImmMap<string,mixed> $values;

    private static array<string> $boolFields = ['background', 'unique', 'sparse'];
    private static array<string> $floatFields = ['min', 'max', 'bucketSize'];
    private static array<string> $intFields = ['bits', '2dsphereIndexVersion', 'textIndexVersion', 'expireAfterSeconds'];
    private static array<string> $stringFields = ['default_language', 'language_override'];
    private static array<string> $documentFields = ['partialFilterExpression', 'storageEngine', 'weights'];

    /**
     * Creates an immutable MongoDB Index creation definition
     *
     * @param $keys - The key definition
     * @param $name - Optional. The index name.
     * @param $options - Optional. Any index creation options.
     * @see https://docs.mongodb.com/manual/reference/command/createIndexes/
     */
    public function __construct(KeyedTraversable<string,mixed> $keys, ?string $name = null, ?KeyedContainer<string,mixed> $options = null)
    {
        $values = Map{
            'key' => is_array($keys) ? $keys : (new Map($keys))->toArray()
        };
        if ($name !== null) {
            $values['name'] = $name;
        }
        if ($options) {
            foreach (self::$boolFields as $v) {
                if (array_key_exists($v, $options)) {
                    $values[$v] = (bool) $options[$v];
                }
            }
            foreach (self::$floatFields as $v) {
                if (array_key_exists($v, $options)) {
                    $values[$v] = (float) $options[$v];
                }
            }
            foreach (self::$intFields as $v) {
                if (array_key_exists($v, $options)) {
                    $values[$v] = (int) $options[$v];
                }
            }
            foreach (self::$stringFields as $v) {
                if (array_key_exists($v, $options)) {
                    $values[$v] = (string) $options[$v];
                }
            }
            foreach (self::$documentFields as $v) {
                if (array_key_exists($v, $options)) {
                    $doc = $options[$v];
                    if ($doc instanceof KeyedTraversable) {
                        $values[$v] = (new Map($doc))->toArray();
                    }
                }
            }
        }
        $this->values = $values->toImmMap();
    }

    /**
     * Gets the array version of this index.
     *
     * @return - The array version
     */
    <<__Memoize>>
    public function toArray(): array<string,mixed>
    {
        return $this->values->toArray();
    }
}
