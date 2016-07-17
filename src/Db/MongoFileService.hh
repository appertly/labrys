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
namespace Labrys\Db;

use MongoDB\BSON\ObjectID;
use MongoDB\GridFS\Bucket;
use Psr\Http\Message\StreamInterface;

/**
 * File upload service backed by GridFS.
 */
class MongoFileService implements \Labrys\Io\FileService<ObjectID,\stdClass>
{
    use MongoHelper;

    /**
     * Creates a new MongoFileService
     *
     * @param $bucket - The GridFS Bucket
     */
    public function __construct(private Bucket $bucket)
    {
    }

    /**
     * Stores an uploaded file.
     *
     * You should specify `contentType` in the `metadata` Map.
     *
     * @param $file - The uploaded file
     * @param $metadata - Any additional fields to persist. At the very least, try to supply `contentType`.
     * @return - The document ID of the stored file
     */
    public function store(\Psr\Http\Message\UploadedFileInterface $file, \ConstMap<string,mixed> $metadata): ObjectID
    {
        $meta = [
            "contentType" => $metadata['contentType'] ?? $file->getClientMediaType(),
            'metadata' => $metadata->toArray()
        ];
        return $this->bucket->uploadFromStream(
            $file->getClientFilename(),
            $file->getStream()->detach(),
            $meta
        );
    }

    /**
     * Gets the file as a PSR-7 Stream.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return - The readable stream
     */
    public function messageStream(mixed $id): StreamInterface
    {
        $file = $this->read($id);
        return new MongoDownloadStream(
            new \MongoDB\GridFS\ReadableStream($this->bucket->getCollectionWrapper(), $file)
        );
    }

    /**
     * Gets a readable stream resource for the given ID.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return - The readable stream
     */
    public function resource(mixed $id): resource
    {
        return $this->bucket->openDownloadStream($id instanceof ObjectID ? $id : new ObjectID((string) $id));
    }

    /**
     * Efficiently writes the contents of a file to a Stream.
     *
     * @param $file - The file
     * @param $stream - The stream
     */
    public function stream(\stdClass $file, StreamInterface $stream): void
    {
        $this->bucket->downloadToStream(
            $file->_id,
            \Labrys\Io\StreamWrapper::getResource($stream)
        );
    }

    /**
     * Gets a stored file.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return - The stored file
     */
    public function read(mixed $id) : ?\stdClass
    {
        $mid = $this->toId($id);
        return $this->doExecute(function (Bucket $bucket) use ($mid) {
            return $bucket->getCollectionWrapper()->findFileById($mid);
        });
    }

    /**
     * Deletes a stored file.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     */
    public function delete(mixed $id): void
    {
        $mid = $this->toId($id);
        $this->doExecute(function (Bucket $bucket) use ($mid) {
            $bucket->delete($mid);
        });
    }

    /**
     * Finds several files by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @return - The objects found
     */
    public function readAll(\ConstMap<string,mixed> $criteria) : Traversable<\stdClass>
    {
        return $this->doExecute(function (Bucket $bucket) use ($criteria) {
            return $bucket->find(
                $criteria->toArray(),
                ['sort' => ['filename' => 1]]
            );
        });
    }

    /**
     * Executes something in the context of the collection.
     *
     * Exceptions are caught and translated.
     *
     * @param $cb - The closure to execute, takes the Bucket
     * @return - Whatever the function returns, this method also returns
     * @throws \Labrys\Db\Exception If a database problem occurs
     */
    protected function doExecute<Ta>((function(Bucket): Ta) $cb) : Ta
    {
        try {
            return $cb($this->bucket);
        } catch (\Exception $e) {
            throw self::translateException($e);
        }
    }
}
