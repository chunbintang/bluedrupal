<?php
require_once './vendor/autoload.php';

use OpenCloud\OpenStack;
use OpenCloud\ObjectStore\Resource\DataObject;

$services = getenv("VCAP_SERVICES");
$services_json = json_decode($services,true);
$obj_config = $services_json["objectstorage"][0]["credentials"];

echo "Try to authenticate Bluemix Object Store... </br>";
$endpoint = $obj_config["auth_uri"];
$credentials = array(
    'username' => $obj_config["username"],
    'password' => $obj_config["password"]
);

$openstack = new OpenStack($endpoint, $credentials);
$openstack->Authenticate();
echo "Bluemix Object Store authenticated </br>";

$objectStoreService = $openstack->objectStoreService('DummyCloudFile', 'DummyRegion', 'tenant');
echo "Try to create Container drupalObjectStore... </br>";
$objectStoreService->createContainer('drupalObjectStore');
$containers=$objectStoreService->listContainers();
$con=FALSE;
foreach ($containers as $container) {
    if($container->getName()=='drupalObjectStore')
	{
	    $con=$container;
		break;
	}
}

if($con)
{
    $container->setReadPublic();
    echo "Container drupalObjectStore created.</br>";
}
else
{
    echo "Fail to create Container drupalObjectStore </br>";
}

?>