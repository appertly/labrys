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
 * Renders an Error Page.
 *
 * Accepts an attribute, `ConstMap<string,mixed> values`, which can contain the
 * following fields: `title`, `detail`, `extra`. The field `extra` itself should
 * be a `KeyedTraversable` which can contain `exception`, 'message', 'stack',
 * and `errors` (which is a `Traversable` that should contain `ConstMap` values
 * that have a `field` and `code`).
 */
class :labrys:error-page extends :x:element
{
    category %flow;
    children empty;
    attribute ConstMap<string,mixed> values = ImmMap{};

    protected function render(): XHPRoot
    {
        $values = $this->:values;
        $frag = <x:frag/>;
        $extra = $values['extra'];
        if ($extra instanceof \ConstMap){
            $errors = $extra->get('errors');
            if ($errors instanceof Traversable) {
                $ul = <ul/>;
                foreach ($errors as $err) {
                    $err = $err instanceof \ConstMap ? $err : ImmMap{};
                    $ul->appendChild(<li>{$err['field'] ?? ''}: {$err['code'] ?? ''}</li>);
                }
                $frag->appendChild($ul);
            }
            if ($extra->containsKey('exception')) {
                $frag->appendChild($this->getExceptionBlock($extra['exception']));
            }
        }
        return <x:doctype>
            <html lang="en">
                <head>
                    <meta charset="utf-8"/>
                    <title>{$values['title']}</title>
                </head>
                <body>
                    <header>
                        <h1>{$values['title']}</h1>
                    </header>
                    <main role="main">
                        <p>{$values['detail']}</p>
                        {$frag}
                    </main>
                </body>
            </html>
        </x:doctype>;
    }

    private function getExceptionBlock(Map<string,mixed> $evalues): XHPRoot
    {
        $frag = <div></div>;
        if ($evalues->containsKey('class')) {
            $frag->appendChild(<h2>{$evalues['class']}</h2>);
        }
        if ($evalues->containsKey('message')) {
            $frag->appendChild(<p>{$evalues['message']}</p>);
        }
        if ($evalues->containsKey('stack')) {
            $frag->appendChild(<pre>{$evalues['stack']}</pre>);
        }
        $p = $evalues['previous'] ?? null;
        if ($p instanceof Map) {
            $frag->appendChild($this->getExceptionBlock($p));
        }
        return $frag;
    }
}
