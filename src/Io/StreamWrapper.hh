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
namespace Labrys\Io;

/**
 * Allows PSR-7 streams to be used as PHP streams.
 *
 * Based on Guzzle PSR7 stream wrapper and MongoDB's PHP library stream wrapper.
 */
class StreamWrapper
{
    /**
     * @var The stream context
     */
    public ?resource $context;

    /**
     * @var The PSR-7 stream
     */
    private ?StreamInterface $stream;

    /**
     * @var The stream mode
     */
    private ?string $mode;

    /**
     * Registers this stream wrapper
     */
    public static function register(): void
    {
        if (!in_array('psr7', stream_get_wrappers())) {
            stream_wrapper_register('psr7', __CLASS__);
        }
    }

    /**
     * Unregisters this stream wrapper
     */
    public static function unregister(): void
    {
        if (in_array('psr7', stream_get_wrappers())) {
            stream_wrapper_unregister('psr7');
        }
    }

    /**
     * Gets a PHP stream resource for the provided PSR-7 stream.
     *
     * @param $stream - The stream to wrap
     * @return - The generated resource
     * @throws \InvalidArgumentException if stream is not readable or writable
     */
    public static function getResource(\Psr\Http\Message\StreamInterface $stream): resource
    {
        self::register();
        if ($stream->isReadable()) {
            $mode = $stream->isWritable() ? 'r+' : 'r';
        } elseif ($stream->isWritable()) {
            $mode = 'w';
        } else {
            throw new \InvalidArgumentException('The stream must be readable, writable, or both');
        }
        return fopen('psr7://stream', $mode, false, stream_context_create([
            'psr7' => ['stream' => $stream]
        ]));
    }

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path)
    {
        $options = stream_context_get_options($this->context);
        if (!isset($options['psr7']['stream'])) {
            return false;
        }
        $this->stream = $options['psr7']['stream'];
        $this->mode = $mode;
        return true;
    }

    public function stream_read(int $count): string
    {
        return $this->stream->read($count);
    }

    public function stream_write(string $data): int
    {
        return (int) $this->stream->write($data);
    }

    public function stream_tell(): int
    {
        return $this->stream->tell();
    }

    public function stream_eof(): bool
    {
        return $this->stream->eof();
    }

    public function stream_seek(int $offset, int $whence = SEEK_SET): bool
    {
        return $this->stream->seek($offset, $whence);
    }

    public function stream_stat(): array<arraykey,int>
    {
        static $modeMap = [
            'r'  => 33060,
            'r+' => 33206,
            'w'  => 33188
        ];
        $stat = $this->getStatTemplate();
        $stat[2] = $stat['mode'] = $modeMap[$this->mode];
        $stat[7] = $stat['size'] = $this->stream->getSize();
        return $stat;
    }

    /**
     * Gets a URL stat template with default values.
     *
     * Blatently lifted from https://github.com/aws/aws-sdk-php/blob/master/src/S3/StreamWrapper.php
     *
     * @return - The stat template.
     */
    private function getStatTemplate(): array<arraykey,int>
    {
        return [
            0  => 0,  'dev'     => 0,
            1  => 0,  'ino'     => 0,
            2  => 0,  'mode'    => 0,
            3  => 0,  'nlink'   => 0,
            4  => 0,  'uid'     => 0,
            5  => 0,  'gid'     => 0,
            6  => -1, 'rdev'    => -1,
            7  => 0,  'size'    => 0,
            8  => 0,  'atime'   => 0,
            9  => 0,  'mtime'   => 0,
            10 => 0,  'ctime'   => 0,
            11 => -1, 'blksize' => -1,
            12 => -1, 'blocks'  => -1,
        ];
    }
}
