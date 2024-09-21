<?php
// Recibir los datos enviados desde el ESP32
if ($_SERVER['REQUEST_METHOD'] === 'POST' || $_SERVER['REQUEST_METHOD'] === 'GET') {
    //    if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    // Obtener las variables enviadas
    //$variable1 = $_POST['variable1'];
    //$variable2 = $_POST['variable2'];

    //$sensor=$_GET['sensor'];
    //$valor = $_GET['valor'];
    // Obtiene la hora actual en la zona horaria de México
    date_default_timezone_set('America/Mexico_City');
    // Obtiene la fecha actual en una variable
    $fechaActual = date("Y-m-d");

    // Obtiene la hora actual en una variable
    $horaActual = date("H:i:s");

    $horaActual = date("H:i:s", strtotime($horaActual . " -1 hour"));
    //echo "La fecha actual es: " . $fechaActual . "<br>";
    //echo "La hora actual es: " . $horaActual;

    $sensor = $_POST['sensor'];
    $valor = $_POST['valor'];

    $sensor2 = $_POST['sensor2'];
    $valor2 = $_POST['valor2'];


    // Guardar las variables en la base de datos (ejemplo)
    // Aquí debes agregar el código para conectarte a tu base de datos y realizar la inserción
    // Por ejemplo, usando MySQLi o PDO

    // Ejemplo con MySQLi
    $conn = new mysqli("localhost", "root", "", "mso");
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    $valor = floatval($valor);

    // Escapar los valores para prevenir inyecciones SQL
    //$nombre_escapado = $conn->real_escape_string($variable1);

    $sql = "INSERT INTO sensors_data (sensor_id, valor,fecha,hora) VALUES ('$sensor', '$valor','$fechaActual','$horaActual')";
    if ($conn->query($sql) === TRUE) {
        echo "Temperatura guardada";
    } else {
        echo "Error al guardar los datos en la base de datos: " . $conn->error;
    }



    $sql = "INSERT INTO sensors_data (sensor_id, valor,fecha,hora) VALUES ('$sensor2', '$valor2','$fechaActual','$horaActual')";

    if ($conn->query($sql) === TRUE) {
        echo "Datos guardados Hz Guardados";
    } else {
        echo "Error al guardar los datos en la base de datos: " . $conn->error;
    }
    $conn->close();


} else {
    echo "Método no permitido";
}
