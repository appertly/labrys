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
 * @license   Apache-2.0
 */
namespace Labrys\View;

use Axe\Page;
use Caridea\Container\EmptyContainer;

/**
 * Creates Views and broadcasts the render event.
 */
class Service implements \Caridea\Container\ContainerAware
{
    use \Caridea\Container\ContainerSetter;

    /**
     * The page
     */
    private ?Page $page;

    /**
     * List of statuses
     */
    private static ImmVector<string> $statuses = ImmVector{'msg-warning', 'msg-info', 'msg-error'};

    /**
     * Creates a new ViewService.
     *
     * @param $container - The dependency injection container
     */
    public function __construct(?\Caridea\Container\Container $container)
    {
        $this->container = $container ?? new EmptyContainer();
    }

    /**
     * Gets the Page for this request (created lazily).
     *
     * @param $title - The page title
     * @return - A Page
     */
    public function getPage(\Stringish $title): Page
    {
        if ($this->page === null) {
            $page = (new Page())->setTitle($this->getPageTitle($title));
            \HH\Asio\join($this->callPageVisitors($page));
            $this->page = $page;
        }
        return $this->page;
    }

    /**
     * Calls the page visitors.
     *
     * @param $page - The page to visit
     */
    protected async function callPageVisitors(Page $page): Awaitable<Vector<mixed>>
    {
        return await \HH\Asio\v(
            array_map(
                (PageVisitor $v) ==> $v->visit($page),
                /* HH_IGNORE_ERROR[4064]: This is never null */
                $this->container->getByType(PageVisitor::class)
            )
        );
    }

    /**
     * Generates a page title.
     *
     * @param $title - The page title
     * @return - The formatted page title
     */
    protected function getPageTitle(?\Stringish $title): string
    {
        return sprintf(
        /* HH_FIXME[4110]: HHVM is weird with this method */
        /* HH_FIXME[4027]: HHVM is weird with this method */
        /* HH_IGNORE_ERROR[4064]: This is never null */
            $this->container->get('web.ui.title.template'),
            $title,
        /* HH_IGNORE_ERROR[4064]: This is never null */
            $this->container->get('system.name')
        );
    }

    /**
     * Sets a Flash Message.
     *
     * @param $name - The status
     * @param $value - The message
     * @param $current - Whether to add message to the current request
     */
    public function setFlashMessage(string $name, string $value, bool $current = false): void
    {
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $session = $this->container->getFirst(\Caridea\Session\Session::class);
        if ($session === null) {
            throw new \UnexpectedValueException("No Session Manager found");
        }
        $session->resume() || $session->start();
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $flash = $this->container->getFirst(\Caridea\Session\FlashPlugin::class);
        if ($flash === null) {
            throw new \UnexpectedValueException("No Flash Plugin found");
        }
        $flash->set($name, $value, $current);
    }

    /**
     * Clears Flash Messages.
     *
     * @param $name - The status
     * @param $value - The message
     * @param $current - Whether to add message to the current request
     */
    public function clearFlashMessages(bool $current = false): void
    {
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $session = $this->container->getFirst(\Caridea\Session\Session::class);
        if ($session === null) {
            throw new \UnexpectedValueException("No Session Manager found");
        }
        $session->resume() || $session->start();
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $flash = $this->container->getFirst(\Caridea\Session\FlashPlugin::class);
        if ($flash === null) {
            throw new \UnexpectedValueException("No Flash Plugin found");
        }
        $flash->clear($current);
    }

    /**
     * Keeps all current flash messages for the next request.
     */
    public function keepFlashMessages(): void
    {
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $session = $this->container->getFirst(\Caridea\Session\Session::class);
        if ($session === null) {
            throw new \UnexpectedValueException("No Session Manager found");
        }
        $session->resume() || $session->start();
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $flash = $this->container->getFirst(\Caridea\Session\FlashPlugin::class);
        if ($flash === null) {
            throw new \UnexpectedValueException("No Flash Plugin found");
        }
        $flash->keep();
    }

    /**
     * Gets any flash messages in the session keyed by status.
     *
     * @return - ImmMap of flash messages
     */
    public function getFlashMessages(): \ConstMap<string,ImmVector<string>>
    {
        /* HH_IGNORE_ERROR[4064]: This is never null */
        $flash = $this->container->getFirst(\Caridea\Session\FlashPlugin::class);
        if ($flash === null) {
            throw new \UnexpectedValueException("No Flash Plugin found");
        }
        $plugin = $flash;
        $map = Map{};
        foreach (self::$statuses as $status) {
            $messages = $plugin->getCurrent($status);
            if (!$messages) {
                continue;
            }
            $vector = Vector{};
            if ($messages instanceof Traversable) {
                foreach ($messages as $message) {
                    $vector->add((string) $message);
                }
            } else {
                $vector->add((string) $messages);
            }
            $map->set($status, $vector->toImmVector());
        }
        return $map->toImmMap();
    }

    /**
     * Gets the last request that the Dispatcher sent to a controller.
     *
     * @return - The last dispatched request, or `null`
     * @since 0.6.3
     */
    public function getDispatchedRequest(): ?\Psr\Http\Message\ServerRequestInterface
    {
        $c = $this->container ?? new EmptyContainer();
        $d = Vector::fromItems($c->getByType(\Labrys\Route\Dispatcher::class))->firstValue();
        return $d?->getLastDispatchedRequest();
    }

    /**
     * Gets all blocks registered for a given region
     *
     * @param $region - The region to search
     * @return - The found blocks in that region, or an empty array.
     */
    public function getBlocks(string $region): \ConstVector<Block>
    {
        $c = $this->container ?? new EmptyContainer();
        $blocks = new Vector($c->getByType(Block::class));
        $blocks = $blocks->filter((Block $b) ==> $region === $b->getRegion());
        return $blocks->toImmVector();
    }

    /**
     * Gets any `Labrys\Db\DbRefResolver` objects in the container.
     *
     * @return - The DbRefResolver objects found.
     * @since 0.5.0
     */
    public function getDbRefResolvers(): \ConstVector<\Labrys\Db\DbRefResolver>
    {
        /* HH_IGNORE_ERROR[4064]: This is never null */
        return new ImmVector($this->container->getByType(\Labrys\Db\DbRefResolver::class));
    }

    /**
     * Gets any `Labrys\View\EntityLinker` objects in the container.
     *
     * @return - The EntityLinker objects found.
     * @since 0.5.0
     */
    public function getEntityLinkers(): \ConstVector<EntityLinker>
    {
        /* HH_IGNORE_ERROR[4064]: This is never null */
        return new ImmVector($this->container->getByType(EntityLinker::class));
    }
}
