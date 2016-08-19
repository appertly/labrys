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
namespace Labrys\Acl;

/**
 * Immutable access control helper.
 *
 * This class determines the user's subjects upon instantiation and stores them
 * for its duration.
 */
class Gatekeeper
{
    private array<\Caridea\Acl\Subject> $subjects;

    /**
     * Creates a new Gatekeeper.
     *
     * @param $aclService - The ACL service
     * @param $principal - The authenticated principal
     * @param $subjectResolvers - Any additional subject resolvers
     */
    public function __construct(
        private \Caridea\Acl\Service $aclService,
        private \Caridea\Auth\Principal $principal,
        array<SubjectResolver> $subjectResolvers
    ) {
        $subjects = Vector{\Caridea\Acl\Subject::principal((string)$principal->getUsername())};
        foreach ($subjectResolvers as $resolver) {
            $subjects->addAll($resolver->getSubjects($principal));
        }
        $this->subjects = $subjects->toArray();
    }

    /**
     * Determines if the currently authenticated user can access the resource.
     *
     * @param $verb - The verb (e.g. 'read', 'write')
     * @param $type - The type of object
     * @param $id - The object identifier
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     */
    public function assert(string $verb, string $type, mixed $id): void
    {
        $this->aclService->assert(
            $this->subjects,
            $verb,
            new \Caridea\Acl\Target($type, $id)
        );
    }

    /**
     * Determines if the currently authenticated user can access the resources.
     *
     * @since 0.5.1
     * @param $verb - The verb (e.g. 'read', 'write')
     * @param $type - The type of object
     * @param $ids - The object identifiers
     * @throws \Caridea\Acl\Exception\Forbidden If the user has no access
     */
    public function assertAll<T>(string $verb, string $type, Traversable<T> $ids): void
    {
        $targets = Vector::fromItems($ids)->map($a ==> new \Caridea\Acl\Target($type, $a));
        $acls = $this->aclService->getAll($targets->toArray(), $this->subjects);
        foreach ($acls as $acl) {
            if (!$acl->can($this->subjects, $verb)) {
                throw new \Caridea\Acl\Exception\Forbidden("Access denied to $verb " . (string)$acl->getTarget());
            }
        }
    }

    /**
     * Determines if the currently authenticated user can access the resource.
     *
     * @param $verb - The verb (e.g. 'read', 'write')
     * @param $type - The type of object
     * @param $id - The object identifier
     * @return - Whether the user has access
     */
    public function can(string $verb, string $type, mixed $id): bool
    {
        return $this->aclService->can(
            $this->subjects,
            $verb,
            new \Caridea\Acl\Target($type, $id)
        );
    }
}
