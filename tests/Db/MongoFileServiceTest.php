<?hh // decl
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

use HackPack\HackUnit\Contract\Assert;

class MongoFileServiceTest
{
    private \MongoDB\BSON\ObjectID $mockId;

    <<Setup>>
    public function setUp()
    {
        $this->mockId = new \MongoDB\BSON\ObjectID('51b14c2de8e185801f000006');
    }

    <<Test>>
    public async function testStore(Assert $assert): Awaitable<void>
    {
        $mockFinfo = new class extends \finfo {
            public function __construct(){}
            public function __destruct(){}
            public function file() { return 'text/html'; }
        };

        $mockStream = new class implements \Psr\Http\Message\StreamInterface {
            public function __toString() { return ''; }
            public function close() {}
            public function detach() {}
            public function getSize() {}
            public function tell() {}
            public function eof() {}
            public function isSeekable(){}
            public function seek($offset, $whence = SEEK_SET){}
            public function rewind(){}
            public function isWritable(){}
            public function write($string){}
            public function isReadable(){}
            public function read($length){}
            public function getContents() {}
            public function getMetadata($key = null) {
                if ($key === 'uri') {
                    return '/tmp/foobar';
                }
            }
        };

        $mockFile = new class($mockStream) implements \Psr\Http\Message\UploadedFileInterface {
            public function __construct(private mixed $stream) {}
            public function getStream() {
                return $this->stream;
            }
            public function moveTo($targetPath) {}
            public function getSize() {}
            public function getError() {}
            public function getClientFilename() {
                return 'my_file.txt';
            }
            public function getClientMediaType() {
                return 'text/html';
            }
        };

        $mockMetaData = Map{"foo" => "bar"};

        $mockGridFS = new class($assert, $this->mockId) extends \MongoDB\GridFS\Bucket {
            public function __construct(private Assert $assert, private mixed $id) {}
            public function uploadFromStream(string $name, ?resource $source, array $data)
            {
                $this->assert->string($name)->is('my_file.txt');
                $this->assert->mixed($data)->looselyEquals(['contentType' => 'text/html', 'metadata' => ['foo' => 'bar']]);
                return $this->id;
            }
        };

        $object = new MongoFileService($mockGridFS, $mockFinfo);

        $assert->mixed($object->store($mockFile, $mockMetaData))->identicalTo($this->mockId);
    }

    <<Test>>
    public async function testRead(Assert $assert): Awaitable<void>
    {
        $mockGridFSFile = new \stdClass();
        $mockFinfo = new class extends \finfo {
            public function __construct(){}
            public function __destruct(){}
        };

        $mockGridFS = new class($mockGridFSFile) extends \MongoDB\GridFS\Bucket {
            public function __construct(private mixed $file) {}
            public function getCollectionsWrapper()
            {
                return new class($this->file) {
                    public function __construct(private mixed $file) {}
                    public function getFilesCollection() {
                        return new class($this->file) {
                            public function __construct(private mixed $file) {}
                            public function findOne(mixed $id)
                            {
                                return $this->file;
                            }
                        };
                    }
                };
            }
        };

        $object = new MongoFileService($mockGridFS, $mockFinfo);
        $assert->mixed($object->read($this->mockId))->identicalTo($mockGridFSFile);
    }
}
