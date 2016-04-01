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
namespace Labrys;

/**
 * Provides convenience methods for getting values out of maps.
 */
class MapWrap<Tk,Tv>
{
    private \ConstMap<Tk,Tv> $map;
    private static Map<Tk,Tv> $empty = Map{};

    /**
     * Creates a new MapWrap.
     *
     * @param $map - The Map to wrap
     */
    public function __construct(KeyedContainer<Tk,Tv> $map)
    {
        $this->map = $map instanceof \ConstMap ? $map : new Map($map);
    }

    /**
     * Gets the original delegate map.
     *
     * @return The original delegate map
     */
    public function getMap(): \ConstMap<Tk,Tv>
    {
        return $this->map;
    }

    /**
     * Whether the delegate map contains the key.
     *
     * @param $key - The key
     * @return Whether this value is contained
     */
    public function containsKey(Tk $key): bool
    {
        return $this->map->containsKey($key);
    }

    /**
     * Convenience method that delegates the Map's get
     *
     * @param $k - The key
     * @return The value found (or null)
     */
    public function get(Tk $key): ?Tv
    {
        return $this->map->get($key);
    }

    /**
     * Gets a value as a trimmed string.
     *
     * @param $key - The key
     * @return A trimmed string
     */
    public function trim(Tk $key): string
    {
        $val = $this->map->get($key);
        return $val === null ? '' : trim((string) $val);
    }

    /**
     * Looks for a Map at the given key; automatically converts arrays.
     *
     * @param $key - The key
     * @return A map (potentially empty)
     */
    public function sub(Tk $key): Map<Tk,Tv>
    {
        $val = $this->map->get($key);
        if ($val instanceof Map) {
            return $val;
        } elseif ($val instanceof \ConstMap) {
            return new Map($val);
        } else {
            return is_array($val) ? new Map($val) : self::$empty;
        }
    }

    /**
     * Looks for a MapWrap at the given key; automatically converts arrays.
     *
     * @param $key - The key
     * @return A MapWrap (potentially empty)
     */
    public function subw(Tk $key): MapWrap<Tk,Tv>
    {
        $val = $this->map->get($key);
        if ($val instanceof \ConstMap) {
            return new MapWrap($val);
        } else {
            return new MapWrap(is_array($val) ? $val : self::$empty);
        }
    }

    /**
     * Gets a value in nested maps.
     *
     * @param $key - The dot-separated key
     * @return The nested value
     */
    public function nested(string $key): mixed
    {
        $last = $this;
        $keys = explode('.', $key);
        foreach ($keys as $i => $k) {
            if ($i == count($keys) - 1) {
                return $last->get($k);
            } else {
                $sub = $last->sub($k);
                if ($sub === self::$empty || $sub->isEmpty()) {
                    return null;
                } else {
                    $last = new MapWrap($sub);
                }
            }
        }
    }

    /**
     * Gets a MapWrap in nested maps.
     *
     * @param $key - The dot-separated key
     * @return The nested MapWrap
     */
    public function nestedw(string $key): MapWrap<arraykey,mixed>
    {
        $val = $this->nested($key);
        return $val instanceof KeyedContainer ? new MapWrap($val) : new MapWrap([]);
    }
}
