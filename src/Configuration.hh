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

/**
 * A bootstrapper for reading in module and configuration info.
 *
 * This class expects a `Traversable` full of class names in the
 * `system.modules` configuration setting. Each class name *must* extend
 * `Labrys\Module` or an `UnexpectedValueException` will be thrown.
 *
 * @since 0.5.0
 */
class Configuration
{
    /**
     * Instantiated modules
     */
    protected ImmVector<Module> $modules;
    /**
     * The config container
     */
    protected Properties $config;

    /**
     * Creates a new Configuration.
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
        $this->modules = $this->createModules($config);
        $this->config = $this->createConfigContainer($config);
    }

    private function createModules(\ConstMap<string,mixed> $config): ImmVector<Module>
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

    private function createConfigContainer(\ConstMap<string,mixed> $config): Properties
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

    /**
     * Gets the configuration settings container.
     *
     * @return - The config container
     */
    public function getConfigContainer(): Properties
    {
        return $this->config;
    }

    /**
     * Gets the loaded modules.
     *
     * @return - The loaded modules
     */
    public function getModules(): ImmVector<Module>
    {
        return $this->modules;
    }
}
