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
namespace Labrys\Http;

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\UploadedFileInterface;

/**
 * Authentication plugin for the middleware dispatcher.
 */
class UploadGuard
{
    /**
     * Creates a new UploadGuard.
     *
     * @param $finfo - The MIME detector
     * @param $fileService - The file service
     */
    public function __construct(private \finfo $finfo)
    {
    }

    /**
     * Gets the list of uploaded files, validating file size.
     *
     * @param $request - The PSR HTTP Request
     * @param $field - The request field containing the files
     * @param $maxSize - The maximum allowed file size
     * @throws \Caridea\Validate\Exception\Invalid if any files aren't valid
     */
    public function getUploadedFiles(Request $request, string $field, ?int $maxSize = null): \ConstVector<UploadedFileInterface>
    {
        $allFiles = $request->getUploadedFiles();
        if (!array_key_exists($field, $allFiles)) {
            throw new \Caridea\Validate\Exception\Invalid([$field => 'REQUIRED']);
        }
        $files = $allFiles[$field];
        $files = $files instanceof Traversable ? $files : Vector{$files};
        foreach ($files as $file) {
            $error = $file->getError();
            if (UPLOAD_ERR_INI_SIZE === $error || UPLOAD_ERR_FORM_SIZE === $error) {
                throw new \Caridea\Validate\Exception\Invalid([$field => 'TOO_LONG']);
            } elseif (UPLOAD_ERR_PARTIAL === $error) {
                throw new \Caridea\Validate\Exception\Invalid([$field => 'TOO_SHORT']);
            } elseif (UPLOAD_ERR_NO_FILE === $error) {
                throw new \Caridea\Validate\Exception\Invalid([$field => 'CANNOT_BE_EMPTY']);
            } elseif (UPLOAD_ERR_NO_TMP_DIR === $error || UPLOAD_ERR_CANT_WRITE === $error) {
                throw new \RuntimeException("Cannot write uploaded file to disk");
            }
            if ($maxSize !== null && $maxSize > 0) {
                $size = $file->getSize();
                if ($size > $maxSize) {
                    throw new \Caridea\Validate\Exception\Invalid([$field => 'TOO_LONG']);
                }
            }
        }
        return new ImmVector($files);
    }

    /**
     * Validates the uploaded files in a request.
     *
     * @param $file - The uploaded file
     * @param $mimeTypes - A set of allowed MIME types (e.g. `image/svg+xml`, 'video/*')
     * @return - The MIME type
     * @throws \Caridea\Validate\Exception\Invalid if the file aren't valid
     */
    public function getMimeType(UploadedFileInterface $file, string $field, ?\ConstSet<string> $mimeTypes = null): string
    {
        $mime = $this->finfo->file($file->getStream()->getMetadata('uri'), FILEINFO_MIME_TYPE);
        if ($mimeTypes !== null && !$mimeTypes->contains($mime)) {
            $match = false;
            foreach ($mimeTypes as $t) {
                if (substr($t, -2, 2) === '/*' &&
                        substr_compare($mime, strstr($t, '/', true), 0, strlen($t) - 2) === 0) {
                    $match = true;
                    break;
                }
            }
            if (!$match) {
                throw new \Caridea\Validate\Exception\Invalid([$field => 'WRONG_FORMAT']);
            }
        }
        return $mime;
    }
}
