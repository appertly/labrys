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
namespace Labrys\Web;

/**
 * Provides an XHP node of content.
 */
interface Block
{
    /**
     * Gets the region in which this block should appear.
     *
     * @return - The region
     */
    public function getRegion(): string;

    /**
     * Gets the display order of this block in its region.
     *
     * @return - The display order
     */
    public function getOrder(): int;

    /**
     * Compose the content.
     *
     * @param $request - The server request
     * @return - The XHP node
     */
    public function compose(?\Psr\Http\Message\ServerRequestInterface $request = null) : Awaitable<\XHPRoot>;
}
