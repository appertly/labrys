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

/**
 * Flash messages using the `axe:heads-up` tag.
 *
 * ```hack
 * <labrys:flash-messages service={$service} />
 * ```
 */
class :labrys:flash-messages extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :xhp:html-element,
        Labrys\View\Service service @required;

    protected function render(): XHPRoot
    {
        $container = <div class="flash-messages"/>;
        foreach ($this->:service->getFlashMessages() as $status => $messages) {
            $status = substr($status, 0, 4) === 'msg-' ? substr($status, 4) : 'info';
            $hu = <axe:heads-up status={$status}/>;
            foreach ($messages as $message) {
                $hu->appendChild(<p>{$message}</p>);
            }
            $container->appendChild($hu);
        }
        return $container;
    }
}
