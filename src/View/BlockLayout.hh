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
 * Stores block settings.
 */
class BlockLayout
{
    private Map<string,Map<string,int>> $blocks = Map{};

    /**
     * Adds a block definition to this layout.
     *
     * @param $region - The block region
     * @param $order - The display order, smallest shows up first
     * @param $name - The name of the block object in the container
     * @return - provides a fluent interface
     */
    public function add(string $region, int $order, string $name): this
    {
        if (!$this->blocks->containsKey($region)) {
            $this->blocks[$region] = Map{};
        }
        $this->blocks[$region][$name] = $order;
        return $this;
    }

    /**
     * Gets the blocks defined in a region.
     *
     * @param $region - The block region
     * @return - The block object names in display order
     */
    public function get(string $region): \ConstVector<string>
    {
        $blocks = new Map($this->blocks[$region] ?? null);
        asort($blocks);
        return $blocks->keys()->immutable();
    }

    /**
     * Gets all block definitions.
     *
     * @return - The block definitions
     */
    public function getAll(): \ConstMap<string,\ConstVector<string>>
    {
        $blocks = Map{};
        foreach ($this->blocks as $region => $names) {
            $a = new Map($names);
            asort($a);
            $blocks[$region] = $a->keys()->immutable();
        }
        return $blocks->immutable();
    }

    /**
     * Adds all the block definitions from another block into this one.
     *
     * @param $other - The other object
     * @return - provides a fluent interface
     */
    public function merge(BlockLayout $other): this
    {
        foreach ($other->blocks as $region => $blocks) {
            if (!$this->blocks->containsKey($region)) {
                $this->blocks[$region] = Map{};
            }
            $this->blocks[$region]->setAll($blocks);
        }
        return $this;
    }
}
