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
namespace Labrys;

/**
 * Helps with accessing fields on unknown values.
 *
 * @since 0.4.0
 */
class Getter
{
    /**
     * Gets the MongoDB-style ID from an object.
     *
     * @param $object - The object
     * @return - The ID found or `null`
     */
    public static function getId(mixed $object): mixed
    {
        if ($object instanceof KeyedContainer) {
            return $object['_id'] ?? null;
        } elseif (is_object($object)) {
            $values = get_object_vars($object);
            invariant($values !== null, 'The values array is null');
            if (array_key_exists('_id', $values)) {
                return $values['_id'];
            } elseif (array_key_exists('id', $values)) {
                return $values['id'];
            } elseif (method_exists($object, 'getId') || method_exists($object, '__call')) {
                /* HH_IGNORE_ERROR[4062]: We checked */
                return $object->getId();
            } elseif (method_exists($object, '__get')) {
                /* HH_IGNORE_ERROR[4062]: We checked */
                return $object->id;
            }
        }
        return null;
    }

    /**
     * Extracts any field from an object.
     *
     * @param $object - The object
     * @return - The value found or `null`
     */
    public static function get(mixed $object, string $field): mixed
    {
        if ($object instanceof KeyedContainer) {
            return $object[$field] ?? null;
        } elseif (is_object($object)) {
            $values = get_object_vars($object);
            invariant($values !== null, 'The values array is null');
            if (array_key_exists($field, $values)) {
                return $values[$field];
            } elseif (method_exists($object, 'get' . ucfirst($field)) || method_exists($object, '__call')) {
                return call_user_func([$object, 'get' . ucfirst($field)]);
            } elseif (method_exists($object, '__get')) {
                /* HH_IGNORE_ERROR[4062]: We checked */
                return $object->id;
            }
        }
        return null;
    }
}
