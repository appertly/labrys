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
use Psr\Http\Message\ServerRequestInterface as Request;

/**
 * A block of content.
 *
 * ```hack
 * <labrys:block block={$block} request={$request} />
 * ```
 */
class :labrys:block extends :x:element implements HasXHPHelpers
{
    use XHPHelpers, XHPAsync;

    category %flow;
    children empty;
    attribute :xhp:html-element,
        Labrys\View\Block block @required;

    protected async function asyncRender(): Awaitable<XHPRoot>
    {
        $block = $this->:block;
        $request = $this->getContext('request');
        $kid = await $block->compose($request instanceof Request ? $request : null);
        return <div class={"block {$block->getRegion()}-block"}>
            {$kid}
        </div>;
    }
}
