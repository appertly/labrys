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
namespace Labrys\Web;

use Psr\Http\Message\ResponseInterface as Response;

/**
 * A trait that can be used by controllers who need to return typical JSON
 */
trait JsonHelper
{
    /**
     * Send something as JSON
     *
     * @param $response - The response
     * @param $payload - The object to serialize
     * @return - The JSON response
     */
    protected function sendJson(Response $response, mixed $payload) : Response
    {
        $response->getBody()->write(json_encode($payload));
        return $response->withHeader('Content-Type', 'application/json');
    }

    /**
     * Send notice that an entity was created.
     *
     * @param $response - The response
     * @param $type - The entity type
     * @param $id - The entity ids
     * @return - The JSON response
     */
    protected function sendCreated(Response $response, string $type, \ConstVector<string> $ids, \ConstMap<string,mixed> $extra = ImmMap{}) : Response
    {
        return $this->sendVerb('created', $response, $type, $ids, $extra)
            ->withStatus(201, "Created");
    }

    /**
     * Send notice that objects were deleted.
     *
     * @param $response - The response
     * @param $type - The entity type
     * @param $ids - The entity ids
     * @return - The JSON response
     */
    protected function sendDeleted(Response $response, string $type, \ConstVector<string> $ids, \ConstMap<string,mixed> $extra = ImmMap{}) : Response
    {
        return $this->sendVerb('deleted', $response, $type, $ids, $extra);
    }

    /**
     * Send notice that objects were updated.
     *
     * @param $response - The response
     * @param $type - The entity type
     * @param $ids - The entity ids
     * @param $extra - Any extra data to serialize
     * @return - The JSON response
     */
    protected function sendUpdated(Response $response, string $type, \ConstVector<string> $ids, \ConstMap<string,mixed> $extra = ImmMap{}) : Response
    {
        return $this->sendVerb('updated', $response, $type, $ids, $extra);
    }

    /**
     * Sends a generic notice that objects have been operated on
     *
     * @param $verb - The verb
     * @param $response - The response
     * @param $type - The entity type
     * @param $ids - The entity ids
     * @param $extra - Any extra data to serialize
     * @return - The JSON response
     */
    protected function sendVerb(string $verb, Response $response, string $type, \ConstVector<string> $ids, \ConstMap<string,mixed> $extra = ImmMap{}) : Response
    {
        $send = new Map($extra);
        $send->setAll(Map{
            'success' => true,
            'message' => "Objects $verb successfully",
            'objects' => $ids->map($id ==> ImmMap{'type' => $type, 'id' => $id})
        });
        return $this->sendJson($response, $send);
    }
}
