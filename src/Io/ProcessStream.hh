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
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */
namespace Labrys\Io;

/**
 * Implementation of PSR HTTP streams for processes
 */
class ProcessStream implements \Psr\Http\Message\StreamInterface
{
    private resource $process;
    private ?resource $stream;

    /**
     * @param $input - The data to write to the process' stdin
     * @param $process - The process to execute
     * @throws InvalidArgumentException
     */
    public function __construct(string $input, string $process)
    {
        $descriptors = [
            0 => ["pipe", "r"],  // stdin is a pipe that the child will read from
            1 => ["pipe", "w"],  // stdout is a pipe that the child will write to
            //2 => ["pipe", "w"],  // stderr is a pipe that the child will write to
        ];
        $pipes = [];
        $this->process = proc_open($process, $descriptors, $pipes);
        if (!is_resource($this->process)) {
            throw new \RuntimeException("Could not execute process");
        }
        fwrite($pipes[0], $input);
        fclose($pipes[0]);
        $this->stream = $pipes[1];
    }

    /**
     * Closes the stream and any underlying resources.
     */
    public function close(): void
    {
        if ($this->stream !== null) {
            $resource = $this->detach();
            fclose($resource);
        }
        proc_close($this->process);
    }

    /**
     * Separates any underlying resources from the stream.
     *
     * After the stream has been detached, the stream is in an unusable state.
     *
     * @return Underlying PHP stream, if any
     */
    public function detach(): ?resource
    {
        $resource = $this->stream;
        $this->stream = null;
        return $resource;
    }

    /**
     * Returns true if the stream is at the end of the stream.
     *
     * @return Whether the stream pointer is at the end
     */
    public function eof(): bool
    {
        return $this->stream === null ? true : feof($this->stream);
    }

    /**
     * Returns the remaining contents in a string.
     *
     * @return The stream contents
     * @throws \RuntimeException if unable to read or an error occurs while reading
     */
    public function getContents(): string
    {
        if (!$this->isReadable()) {
            throw new \RuntimeException('Cannot read from stream');
        }
        $result = stream_get_contents($this->stream);
        if ($result === false) {
            throw new \RuntimeException('Error reading from stream');
        }
        return $result;
    }

    /**
     * Get stream metadata as an associative array or retrieve a specific key.
     *
     * The keys returned are identical to the keys returned from PHP's
     * `stream_get_meta_data` function.
     *
     * @param $key - Specific metadata to retrieve.
     * @return Returns an associative array if no key is provided. Returns a
     *     specific key value if a key is provided and the value is found, or
     *     null if the key is not found.
     */
    public function getMetadata(?string $key = null): mixed
    {
        if ($key === null) {
            return stream_get_meta_data($this->stream);
        } else {
            $metadata = stream_get_meta_data($this->stream);
            return $metadata[$key] ?? null;
        }
    }

    /**
     * Get the size of the stream if known.
     *
     * @return Returns the size in bytes if known, or null if unknown.
     */
    public function getSize(): ?int
    {
        return null;
    }

    /**
     * Returns whether or not the stream is readable.
     *
     * @return Whether or not the stream is readable
     */
    public function isReadable(): bool
    {
        return is_resource($this->stream);
    }

    /**
     * Returns whether or not the stream is seekable.
     *
     * @return Whether the stream is seekable
     */
    public function isSeekable(): bool
    {
        return false;
    }

    /**
     * Returns whether or not the stream is writable.
     *
     * @return Whether the stream is writable
     */
    public function isWritable(): bool
    {
        return false;
    }

    /**
     * Read data from the stream.
     *
     * @param $length - Read up to $length bytes from the object and return
     *     them. Fewer than $length bytes may be returned if underlying stream
     *     call returns fewer bytes.
     * @return Returns the data read from the stream, or an empty string if no
     *     bytes are available
     * @throws \RuntimeException if an error occurs
     */
    public function read($length): string
    {
        if (!$this->isReadable()) {
            throw new \RuntimeException('Cannot read from stream');
        }
        $result = fread($this->stream, $length);
        if ($result === false) {
            throw new \RuntimeException('Error reading stream');
        }
        return $result;
    }

    /**
     * Seek to the beginning of the stream.
     *
     * This stream is not seekable, this method will raise an exception if the
     * stream pointer is not at the beginning.
     *
     * @throws \RuntimeException on failure.
     */
    public function rewind(): void
    {
        if ($this->tell() === 0) {
            return;
        }
        throw new \BadMethodCallException('Stream is not seekable');
    }

    /**
     * Seek to a position in the stream.
     *
     * @param $offset - Stream offset
     * @param $whence - Specifies how the cursor position will be calculated
     *     based on the seek offset. Valid values are identical to the built-in
     *     PHP $whence values for `fseek()`.  SEEK_SET: Set position equal to
     *     offset bytes SEEK_CUR: Set position to current location plus offset
     *     SEEK_END: Set position to end-of-stream plus offset.
     * @throws \RuntimeException on failure.
     */
    public function seek($offset, $whence = SEEK_SET): void
    {
        if ($offset === 0 && ($this->tell() === 0 || $whence !== SEEK_SET)) {
            return;
        }
        throw new \BadMethodCallException('Stream is not seekable');
    }

    /**
     * Returns the current position of the file read/write pointer
     *
     * @return Position of the file pointer
     * @throws \RuntimeException on error.
     */
    public function tell(): int
    {
        if ($this->stream === null) {
            throw new \RuntimeException('Cannot determine stream pointer position');
        }
        $result = ftell($this->stream);
        if ($result === false) {
            throw new \RuntimeException('Cannot determine stream pointer position');
        }
        return $result;
    }

    /**
     * Write data to the stream.
     *
     * This stream is not writable, this method will raise an exception.
     *
     * @param $string - The string that is to be written
     * @return Returns the number of bytes written to the stream.
     * @throws \RuntimeException on failure
     */
    public function write($string): int
    {
        throw new \BadMethodCallException('Stream is not writable');
    }

    /**
     * Reads all data from the stream into a string, from the beginning to end.
     *
     * This method MUST attempt to seek to the beginning of the stream before
     * reading data and read the stream until the end is reached.
     *
     * Warning: This could attempt to load a large amount of data into memory.
     *
     * This method MUST NOT raise an exception in order to conform with PHP's
     * string casting operations.
     *
     * @see http://php.net/manual/en/language.oop5.magic.php#object.tostring
     * @return string
     */
    public function __toString(): string
    {
        if (!$this->isReadable()) {
            return '';
        }
        try {
            $this->rewind();
            return $this->getContents();
        } catch (\Exception $e) {
            return '';
        }
    }
}
