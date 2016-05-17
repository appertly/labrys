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
namespace Labrys\Db;

/**
 * Can be used by any class which accesses MongoDB.
 */
trait MongoDateConversion
{
    /**
     * Tries to convert a `UTCDateTime` into a `DateTime`.
     *
     * @param $value - The value to convert
     * @param $timeZone - Optional Time Zone to set
     * @return - The date if `$value` is a `UTCDateTime`, `null` otherwise
     */
    protected function toDateTime(mixed $value, ?\DateTimeZone $timeZone = null): ?\DateTime
    {
        if ($value instanceof \MongoDB\BSON\UTCDateTime) {
            $date = $value->toDateTime();
            if ($timeZone !== null) {
                $date->setTimezone($timeZone);
            }
            return $date;
        }
        return null;
    }

    /**
     * Tries to convert a `UTCDateTime` into a `DateTimeImmutable`.
     *
     * @param $value - The value to convert
     * @return - The date if `$value` is a `UTCDateTime`, `null` otherwise
     */
    protected function toDateTimeImmutable(mixed $value): ?\DateTimeImmutable
    {
        return $value instanceof \MongoDB\BSON\UTCDateTime ?
            new \DateTimeImmutable('@' . $value->toDateTime()->getTimestamp()) : null;
    }
}
