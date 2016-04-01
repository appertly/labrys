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
 * Error page XHP tag
 */
class :page:error extends :x:element
{
    children (pcdata | %flow)*;
    attribute
        \Stringish site @required;

    protected function render() : XHPRoot
    {
        return <x:doctype>
            <html lang="en">
                <head>
                    <meta charset="utf-8"/>
                    <meta name="viewport" content="width=device-width, initial-scale=1"/>
                    <title>{$this->:site}</title>
                    <link href="//maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet"/>
                    <script src="//oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
                    <script src="//oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
                </head>
                <body>
                    <header class="navbar navbar-inverse">
                        <div class="container">
                            <div class="navbar-header">
                                <a class="navbar-brand" href="/" rel="top">{$this->:site}</a>
                            </div>
                        </div>
                    </header>
                    <main class="jumbotron">
                        <div class="container">
                            {$this->getChildren()}
                        </div>
                    </main>
                    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
                    <script src="//maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
                </body>
            </html>
        </x:doctype>;
    }
}
