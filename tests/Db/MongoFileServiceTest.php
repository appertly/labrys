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
use Mockery as M;

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
        $mockStream = M::mock(\Psr\Http\Message\StreamInterface::class);
        $mockStream->shouldReceive('getMetadata')->withArgs(['uri'])->andReturn('/tmp/foobar');
        $mockStream->shouldReceive('detach')->andReturn(fopen('php://memory', 'r+'));

        $mockFile = M::mock(\Psr\Http\Message\UploadedFileInterface::class);
        $mockFile->shouldReceive('getStream')->andReturn($mockStream);
        $mockFile->shouldReceive('getClientFilename')->andReturn('my_file.txt');
        $mockFile->shouldReceive('getClientMediaType')->andReturn('text/html');

        $mockMetaData = Map{"foo" => "bar", 'contentType' => 'text/html'};

        $expectedData = ['contentType' => 'text/html', 'metadata' => ['foo' => 'bar', 'contentType' => 'text/html']];
        $mockGridFS = M::mock(\MongoDB\GridFS\Bucket::class);
        $mockGridFS->shouldReceive('uploadFromStream')->withArgs(['my_file.txt', M::type('resource'), $expectedData])->andReturn($this->mockId);

        $object = new MongoFileService($mockGridFS);

        $assert->mixed($object->store($mockFile, $mockMetaData))->identicalTo($this->mockId);
        M::close();
    }

    <<Test>>
    public async function testRead(Assert $assert): Awaitable<void>
    {
        $mockGridFSFile = new \stdClass();

        $cw = M::mock(\MongoDB\GridFS\CollectionWrapper::class);
        $cw->shouldReceive('findFileById')->withArgs([$this->mockId])->andReturn($mockGridFSFile);
        $mockGridFS = M::mock(\MongoDB\GridFS\Bucket::class);
        $mockGridFS->collectionWrapper = $cw;

        $object = new MongoFileService($mockGridFS);
        $assert->mixed($object->read($this->mockId))->identicalTo($mockGridFSFile);

        M::close();
    }
}
