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
 * The system bootstrapper.
 */
class System
{
    /**
     * @var Instantiated modules
     */
    private ImmVector<Module> $modules;
    /**
     * @var The config container
     */
    private Properties $config;
    /**
     * @var The backend container
     */
    private Objects $backend;
    /**
     * @var The frontend container
     */
    private Objects $frontend;

    /**
     * Creates a new System.
     *
     * @param $config - The system configuration
     */
    public function __construct(\ConstMap<string,mixed> $config)
    {
        $this->modules = $this->createModules($config);
        $this->config = $this->createConfigContainer($config);
        $this->backend = $this->createBackendContainer($this->config);
        $this->frontend = $this->createFrontendContainer($this->backend);
    }

    private function createModules(\ConstMap<string,mixed> $config) : ImmVector<Module>
    {
        $modules = Vector{};
        $sysModules = $config->get('system.modules');
        if ($sysModules instanceof Traversable) {
            foreach ($sysModules as $className) {
                if (!is_a($className, \Labrys\Module::class, true)) {
                    throw new \UnexpectedValueException("Not a module class: '$className'");
                } else {
                    /* HH_IGNORE_ERROR[4026]: This works as intended */
                    $modules[] = new $className();
                }
            }
        }
        return $modules->toImmVector();
    }

    private function createConfigContainer(\ConstMap<string,mixed> $config) : Properties
    {
        $sysConfig = Map{};
        // first set module defaults
        foreach ($this->modules as $module) {
            $sysConfig->setAll($module->getConfig());
        }
        // then bring in user-specified values
        $sysConfig->setAll($config);
        return new Properties($sysConfig->toArray());
    }

    private function createBackendContainer(Properties $parent) : Objects
    {
        $builder = Objects::builder();
        foreach ($this->modules as $module) {
            $module->setupBackend($builder, $parent);
        }
        return $builder->build($parent);
    }

    private function createFrontendContainer(Objects $parent) : Objects
    {
        $builder = Objects::builder();
        foreach ($this->modules as $module) {
            $module->setupFrontend($builder, $this->config);
        }
        return $builder->build($parent);
    }

    /**
     * Gets the configuration settings container.
     *
     * @return - The config container
     */
    public function getConfigContainer() : Properties
    {
        return $this->config;
    }

    /**
     * Gets the container with backend classes.
     *
     * @return - The backend container
     */
    public function getBackendContainer() : Objects
    {
        return $this->backend;
    }

    /**
     * Gets the container with frontend classes.
     *
     * @return - The frontend container
     */
    public function getFrontendContainer() : Objects
    {
        return $this->frontend;
    }

    /**
     * Gets the loaded modules.
     *
     * @return - The loaded modules
     */
    public function getModules() : ImmVector<Module>
    {
        return $this->modules;
    }
}
