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
namespace Labrys\Db;

/**
 * Can be used by any class which accesses MongoDB.
 */
trait MongoHelper
{
    /**
     * Transforms a literal into a MongoDB ObjectId.
     *
     * @param mixed $id If it's an `ObjectID`, returns that, otherwise creates a
     *        new `ObjectID`.
     * @return The ObjectID
     */
    protected function toId(mixed $id) : \MongoDB\BSON\ObjectID
    {
        return $id instanceof \MongoDB\BSON\ObjectID ? $id : new \MongoDB\BSON\ObjectID((string)$id);
    }

    /**
     * Gets the current time.
     *
     * @return The current time
     */
    protected function now(): \MongoDB\BSON\UTCDateTime
    {
        $parts = explode(' ', microtime());
        return new \MongoDB\BSON\UTCDateTime(sprintf('%d%03d', $parts[1], $parts[0] * 1000));
    }

    /**
     * Tries to parse a date.
     *
     * @param $date - The possible string date value, a string, a
     *        `\DateTimeInterface`, or a `\MongoDB\BSON\UTCDateTime`
     * @return The MongoDB datetime or null
     */
    protected function toDate(mixed $date) : ?\MongoDB\BSON\UTCDateTime
    {
        if ($date instanceof \MongoDB\BSON\UTCDateTime) {
            return $date;
        } elseif ($date instanceof \DateTimeInterface) {
            return new \MongoDB\BSON\UTCDateTime($date->getTimestamp() * 1000);
        } else {
            $date = trim((string)$date);
            return strlen($date) > 0 ? new \MongoDB\BSON\UTCDateTime(strtotime($date) * 1000) : null;
        }
    }

    /**
     * Makes sure a document isn't null.
     *
     * @param $id - The document identifier, either a `\MongoDB\BSON\ObjectID` or string
     * @param $document - The document to check
     * @return Returns `$document`
     * @throws Exception\Retrieval if the document is null
     */
    protected function ensure<Ta>(mixed $id, ?Ta $document) : Ta
    {
        if ($document === null) {
            /* HH_FIXME[4110]: This is stringish */
            throw new Exception\Retrieval("Could not find document with ID $id");
        }
        return $document;
    }

    /**
     * Translates a Mongo exception.
     *
     * @param \Exception $e The exception to translate
     * @return \Labrys|Db\Exception The exception to use
     */
    protected static function translateException(\Exception $e): \Exception
    {
        // TODO exception stuff
        if ($e instanceof \MongoDB\Driver\Exception\RuntimeException &&
            ($e->getCode() == 11000 || 'E11000' == substr($e->getMessage(), 0, 6))) {
            return new Exception\Integrity("Unique constraint violation", 409, $e);
        }
        return new Exception\System("Uncategorized database error", 0, $e);
    }
}
