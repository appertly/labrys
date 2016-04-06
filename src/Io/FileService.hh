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
namespace Labrys\Io;

/**
 * Interface for storing and retrieving uploaded files.
 */
interface FileService<Tk,Tv>
{
    /**
     * Stores an uploaded file.
     *
     * @param $file - The uploaded file
     * @param $metadata - Any additional fields to persist
     * @return The document ID of the stored file
     */
    public function store(\Psr\Http\Message\UploadedFileInterface $file, \ConstMap<string,mixed> $metadata) : Tk;

    /**
     * Gets the file as a PSR-7 Stream.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return The readable stream
     */
    public function messageStream(mixed $id): \Psr\Http\Message\StreamInterface;

    /**
     * Gets a stream resource for the given ID.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return The stream
     */
    public function resource(mixed $id): resource;

    /**
     * Efficiently writes the contents of a file to a Stream.
     *
     * @param $file - The file
     * @param $stream - The stream
     */
    public function stream(Tv $file, \Psr\Http\Message\StreamInterface $stream): void;

    /**
     * Gets a stored file.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     * @return The stored file
     */
    public function read(mixed $id) : ?Tv;

    /**
     * Finds several files by some arbitrary criteria.
     *
     * @param $criteria - Field to value pairs
     * @return The objects found
     */
    public function readAll(\ConstMap<string,mixed> $criteria) : \Traversable<Tv>;

    /**
     * Deletes a stored file.
     *
     * @param $id - The document identifier, either a string or `ObjectID`
     */
    public function delete(mixed $id): void;
}
