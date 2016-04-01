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

/**
 * Base layout XHP tag
 */
class :layout:base extends :x:element
{
    children (pcdata | %flow)*;
    attribute Labrys\Web\Page page @required;

    protected function render() : XHPRoot
    {
        $page = $this->:page;

        $head = <head>
            <meta charset={$page->getEncoding()} />
            <title>{$page->getTitle()}</title>
        </head>;

        foreach ($page->getMeta() as $meta) {
            $head->appendChild($meta);
        }
        foreach ($page->getHeadScripts() as $script) {
            $head->appendChild($script);
        }
        foreach ($page->getLinks() as $link) {
            $head->appendChild($link);
        }

        $body = <body>
            {$this->getChildren()}
            <ui:body-scripts page={$page}/>
        </body>;

        foreach ($page->getBodyClasses() as $class) {
            $body->addClass($class);
        }
        if ($page->getBodyId()) {
            $body->setAttribute('id', $page->getBodyId());
        }

        return <x:doctype>
            <html lang={$page->getLang()}>
                {$head}
                {$body}
            </html>
        </x:doctype>;
    }
}
