<?hh
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
namespace Labrys\Io;

class MongoFileServiceTest extends \PHPUnit_Framework_TestCase
{
    private $mockId;

    protected function setUp()
    {
        $this->mockId = new \MongoDB\BSON\ObjectID('51b14c2de8e185801f000006');
    }

    public function testStore()
    {
        $mockFinfo = $this->getMock(\finfo::class, ['file', '__destruct'], [], '', false);
        $mockFinfo->expects(self::any())
            ->method('file')
            ->willReturn('text/html');
        $mockFinfo->expects(self::any())
            ->method('__destruct')
            ->willReturn(true);

        $mockStream = $this->getMockForAbstractClass(\Psr\Http\Message\StreamInterface::class);
        $mockStream->expects(self::once())
            ->method('getMetadata')
            ->with('uri')
            ->willReturn('/tmp/foobar');
        $mockFile = $this->getMockForAbstractClass(\Psr\Http\Message\UploadedFileInterface::class);
        $mockFile->expects(self::any())
            ->method('getClientMediaType')
            ->willReturn('text/html');
        $mockFile->expects(self::once())
            ->method('getStream')
            ->willReturn($mockStream);
        $mockFile->expects(self::once())
            ->method('getClientFilename')
            ->willReturn('my_file.txt');

        $mockMetaData = Map{"foo" => "bar"};

        $mockGridFS = $this->getMock(\MongoDB\GridFS\Bucket::class, ['storeFile'], [], '', false);
        $mockGridFS->expects(self::any())
            ->method('storeFile')
            ->with(
                '/tmp/foobar',
                ['filename' => 'my_file.txt', 'contentType' => 'text/html', 'metadata' => $mockMetaData->toArray()]
            )
            ->willReturn($this->mockId);

        $MongoFileService = new MongoFileService($mockGridFS, $mockFinfo);

        $this->assertEquals($this->mockId, $MongoFileService->store($mockFile, $mockMetaData));
        $this->verifyMockObjects();
    }

    public function testRead()
    {   $mockGridFSFile = new \stdClass();
        $mockFinfo = $this->getMock(\finfo::class, [], [], '', false);

        $mockGridFS = $this->getMock(\MongoDB\GridFS\Bucket::class, ['findOne'], [], '', false);
        $mockGridFS->expects(self::any())
        ->method('findOne')
        ->willReturn($mockGridFSFile);

        $MongoFileService = new MongoFileService($mockGridFS, $mockFinfo);

        $this->assertEquals($mockGridFSFile, $MongoFileService->read($this->mockId));
        $this->verifyMockObjects();
    }
}
