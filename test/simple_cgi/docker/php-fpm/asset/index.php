<!DOCTYPE html>
<html>
<head>
    <title>Image Upload</title>
</head>
<body>
    <h2>Image Upload</h2>
    <form method="POST" enctype="multipart/form-data">
        <input type="file" name="image">
        <input type="submit" value="Upload">
    </form>

    <?php
    if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['image'])) {
        $uploadDir = '/var/www/html/php/.media/'; // Directory to save the uploaded images
        $uploadedFile = $_FILES['image']['tmp_name'];
        $fileName = $_FILES['image']['name'];

        // Move the uploaded file to the desired location
        if (move_uploaded_file($uploadedFile, $uploadDir . $fileName)) {
            echo 'Image uploaded successfully.';
        } else {
            echo 'Failed to upload the image.';
        }
    }
    ?>
</body>
</html>