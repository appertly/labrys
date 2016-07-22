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
namespace Labrys;

use Caridea\Container\Properties;
use Caridea\Container\Objects;

/**
 * A bootstrapper that reads configuration and creates backend and frontend containers.
 *
 * This class expects a `Traversable` full of class names in the
 * `system.modules` configuration setting. Each class name *must* extend
 * `Labrys\Module` or an `UnexpectedValueException` will be thrown.
 */
class System extends Configuration
{
    /**
     * The backend container
     */
    protected Objects $backend;
    /**
     * The frontend container
     */
    protected Objects $frontend;

    /**
     * Creates a new System.
     *
     * This constructor expects a `Traversable` full of class names in the
     * `system.modules` configuration setting. Each class name *must* extend
     * `Labrys\Module` or an `UnexpectedValueException` will be thrown.
     *
     * @param $config - The system configuration
     * @throws \UnexpectedValueException if a module class doesn't extend `Labrys\Module`
     */
    public function __construct(\ConstMap<string,mixed> $config)
    {
        parent::__construct($config);
        $this->backend = $this->createBackendContainer($this->config);
        $this->frontend = $this->createFrontendContainer($this->backend);
    }

    private function createBackendContainer(Properties $parent) : Objects
    {
        $builder = Objects::builder();
        foreach ($this->modules as $module) {
            $module->setupBackend($builder, $parent);
        }
        return $builder->build($parent);
    }

    private function createFrontendContainer(Objects $parent): Objects
    {
        $builder = Objects::builder();
        foreach ($this->modules as $module) {
            $module->setupFrontend($builder, $this->config);
        }
        return $builder->build($parent);
    }

    /**
     * Gets the container with backend classes.
     *
     * @return - The backend container
     */
    public function getBackendContainer(): Objects
    {
        return $this->backend;
    }

    /**
     * Gets the container with frontend classes.
     *
     * @return - The frontend container
     */
    public function getFrontendContainer(): Objects
    {
        return $this->frontend;
    }
}
