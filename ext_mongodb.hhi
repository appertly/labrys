<?hh // decl

/**
 * This is the public API of the new MongoDB driver for use with the Hack type
 * checker. This file should never be included.
 */

namespace MongoDB\Driver;

final class WriteConcernError {

	public function getCode() : int {}

	public function getMessage() : string { }

	public function getInfo() : ?array { }

	public function __debugInfo() : array { }
}

final class WriteError {

	public function getMessage() {}

	public function getCode() { }

	public function getIndex() { }

	public function getInfo() { }

	public function __debugInfo() { }
}

final class WriteResult {

	public function __wakeup() { }

	public function getInsertedCount() {  }
	public function getMatchedCount()  { }
	public function getModifiedCount() {  }
	public function getDeletedCount()  { }
	public function getUpsertedCount() {  }

	public function getServer() : Server;

	public function getUpsertedIds(): array { }

	public function getWriteConcernError() { }

	public function getWriteErrors(): array { }

	public function isAcknowledged() : bool;

	public function __debugInfo() : array { }
}

class Manager {

	public function __construct(string $dsn = "mongodb://localhost", array $options = array(), array $driverOptions = array());

	public function __debugInfo() : array;

	public function executeCommand(string $db, Command $command, ?ReadPreference $readPreference = null): Cursor;

	public function executeQuery(string $namespace, Query $query, ?ReadPreference $readPreference = null): Cursor;

	public function executeBulkWrite(string $namespace, BulkWrite $bulk, ?WriteConcern $writeConcern = null): WriteResult;

	public function getServers(): array;

	public function getReadPreference() : ReadPreference;

	public function getWriteConcern() : WriteConcern;

	public function __wakeUp() : void;

	public function selectServer(ReadPreference $readPreference): Server;
}

class Utils {
	const ERROR_INVALID_ARGUMENT  = 1;
	const ERROR_RUNTIME           = 2;
	const ERROR_MONGOC_FAILED     = 3;
	const ERROR_WRITE_FAILED      = 4;
	const ERROR_CONNECTION_FAILED = 5;

	static public function throwHippoException($domain, $message) { }

	static public function mustBeArrayOrObject(string $name, mixed $value, string $context = '') { }
}

final class CursorId {
	public function __debugInfo() : array;

	public function __toString() : string;
}

final class Cursor<T> implements \Traversable<T>, \Iterator<T> {
	public function __debugInfo() : array;

	public function getId() : CursorId;

	public function getServer() : Server;

	public function isDead() : bool;

	public function current(): T;

	public function key(): int;

	public function next(): void;

	public function rewind(): void;

	public function valid(): bool;

	public function toArray(): array<T>;

	public function setTypeMap(array<string,?string> $typemap): void;
}

final class Command {

	public function __construct(mixed $command) { }

	public function __debugInfo() { }
}

final class Query {

	public function __construct(mixed $filter, array $options = array()) { }

	public function __debugInfo() : array { }
}

final class BulkWrite implements \Countable {

	public function __construct(?array $bulkWriteOptions = array());


	public function insert(mixed $document) : mixed;


	public function update(mixed $query, mixed $newObj, ?array $updateOptions = array()) : void;


	public function delete(mixed $query, ?array $deleteOptions = array()) : void;


	public function count() : int;


	public function __debugInfo() : array;
}


final class ReadPreference {
	const RP_PRIMARY = 1;
    const RP_PRIMARY_PREFERRED = 5;
    const RP_SECONDARY = 2;
    const RP_SECONDARY_PREFERRED = 6;
    const RP_NEAREST = 10;

	public function __construct(int $readPreference, ?array $tagSets = null) { }

	public function getMode() : int;

	public function getTagSets() : array;

	public function __debugInfo() : array;
}

final class Server {

	public function __debugInfo() : array;

	public function getHost(): string;


	final public function getInfo(): array;


	public function getLatency() : int;


	public function getPort(): int;


	public function getTags() : array;


	public function getType(): int;


	public function isPrimary() : bool;


	public function isSecondary() : bool;


	public function isArbiter() : bool;


	public function isHidden() : bool;


	public function isPassive() : bool;


	public function executeBulkWrite(string $namespace, BulkWrite $bulk, ?WriteConcern $writeConcern = null): WriteResult;


	public function executeCommand(string $db, Command $command, ?ReadPreference $readPreference = null): Cursor;


	public function executeQuery(string $namespace, Query $query, ?ReadPreference $readPreference = null): Cursor;
}

final class WriteConcern {
	const MAJORITY = "majority";

	public function __construct(mixed $w, ?int $wtimeout = 0, ?bool $journal = NULL);


	public function getJournal() : mixed;


	public function getW() : mixed;


	public function getWtimeout() : int;


	public function __debugInfo() : array;
}


namespace MongoDB\Driver\Exception;

interface Exception {}

class ConnectionException extends RuntimeException {}

class AuthenticationException extends ConnectionException {}
class ConnectionTimeoutException extends ConnectionException {}
class ExecutionTimeoutException extends RuntimeException {}
class InvalidArgumentException extends \InvalidArgumentException implements Exception {}
class LogicException extends \LogicException implements Exception {}
class RuntimeException extends \RuntimeException implements Exception {}
class SSLConnectionException extends ConnectionException {}
class UnexpectedValueException extends \UnexpectedValueException implements Exception {}
class BulkWriteException extends WriteException {}

abstract class WriteException extends RuntimeException
{
	final public function getWriteResult() : \MongoDB\Driver\WriteResult { }
}



namespace MongoDB\BSON;

function fromPHP(mixed $data) : string;

function fromJson(string $data) : mixed;

function toPHP(string $data, ?array $typemap = array()) : mixed;

function toJson(string $data) : mixed;

trait DenySerialization
{
	public function serialize() : string { }

	public function unserialize(mixed $data) : void { }
}

interface Type
{
}

interface Serializable extends Type
{
	public function bsonSerialize() : array;
}

interface Unserializable
{
	public function bsonUnserialize(array $data) : void;
}

interface Persistable extends Serializable, Unserializable
{
}

final class Binary implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(string $data, int $type) { }

	public function getType() { }

	public function getData() : string { }

	public function __debugInfo() : array;
}

final class Javascript implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(string $code, mixed $scope = NULL) { }

	public function __debugInfo() : array { }
}

final class MaxKey implements Type, \Serializable
{
	use DenySerialization;
}

final class MinKey implements Type, \Serializable
{
	use DenySerialization;
}

final class ObjectID implements Type, \Serializable
{
	use DenySerialization;


	public function __construct(?string $objectId = null);


	public function __toString() : string;


	public function __debugInfo() : array;
}

final class Regex implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(string $pattern, string $flags) { }

	public function getPattern() : string { }

	public function getFlags() : string { }

	public function __toString() : string { }

	public function __debugInfo() : array { }
}

final class Timestamp implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(int $increment, int $timestamp) { }

	public function __toString() : string { }

	public function __debugInfo() : array { }
}

final class UTCDateTime implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(mixed $milliseconds);

	public function __toString() : string { }

	public function toDateTime() : \DateTime;

	public function __debugInfo() : array { }
}
