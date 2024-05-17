<?php
    // fonctions
    
    // récupère l'entête d'un fichier C
    function get_entete($file) {
        $file = fopen($file, "r") or die("Unable to open file!" . $file);
        $line = fgets($file);
        $pattern = '/\s*\*\s*(.*\n)/'; // pattern pour récupérer le texte du commentaire en C
        if (substr($line, 0, 3) != "/**") {
            return false;
        }
        $header = "";
        $line = fgets($file);
        while (substr($line, 0, 2) != "*/") {
            preg_match($pattern, $line, $matches);
            $header .= $matches[1];
            $line = fgets($file);
        }
        return $header;
    }

    // récupère les #define d'un fichier C
    function get_define($filename) {
        $pattern = "/\s*(#define\s\s*.*)\s*(\/\*\*(.*)\*\/)/";
        preg_match_all($pattern, file_get_contents($filename), $matches);
        return $matches;
    }
    
    // récupère les typedef struct d'un fichier C
    function get_struct($filename) {
        $pattern = "/typedef struct\s*\{([^{}]*)\}\s*(\w*);\s*\/\*\*\s*(.*)\*\//";
        preg_match_all($pattern, file_get_contents($filename), $matches);
        
        foreach ($matches[1] as $key => $attrs) {
    
            $attributes = explode("\n", trim($attrs));
            $matches[1][$key] = Array();
            $matches[1][$key][0] = Array();
            $matches[1][$key][1] = Array();
    
            $i = 0;
            foreach ($attributes as $attribute) {
                $attribute = trim($attribute);
                $matches[1][$key][$i] = separate_struct_attributes($attribute);
                $i++;
            }
        }
    
        return $matches;
    }
    
    // sépare les attributs d'une structure de leur commentaire et les met dans un tableau
    function separate_struct_attributes($line) {
        $pattern = "/(\w* \w*;)\s*\/\*\*\s*(.*)\s*\*\//";
        preg_match_all($pattern, $line, $matches);
        return $matches;
    }

    // récupère les variables globales d'un fichier C
    function get_globales($filename) {
        $pattern = "/(const)?\s*(\w*)\s*(\w*)\s*=\s*(.*)\s*;\s*\/\*\*\s*(.*)\s*\*\//";
        preg_match_all($pattern, file_get_contents($filename), $matches);
        return $matches;
    }

    // récupère les fonctions d'un fichier C
    function get_fonctions($filename) {
        $pattern = "/\s\/\*\*(\s*\*\s*.*\s)+((\w*)?\s*(\w*)\s*(\w*)\s*\((.*)\)\s*\{)/";
        preg_match_all($pattern, file_get_contents($filename), $matches);
        return $matches;
    }
    
    // traite les commentaires des fonctions pour les rendre lisibles
    function process_func_comment($comment) {
        $comment = str_replace("/**","", $comment);
        $comment = str_replace("*/","", $comment);
        
        //remove the stars at the beginning of each line
        $pattern = "/\s*\*\s+(.*\s)/";
        preg_match_all($pattern, $comment, $matches);
        
        $comment = "";
        foreach ($matches[1] as $key => $value) {
            if (substr($value, 0, 2) == "* ") {
                $comment .= substr($value, 2);
            } else {
            $comment .= $value;
            }
        }
        
        $comment = str_replace("\\param", "\nParamètre :", trim($comment));
        $comment = str_replace("\\return", "\nRetourne :", trim($comment));
        $comment = str_replace("\\brief", "\nDescription :", trim($comment));
        $comment = str_replace("\\detail", "\nDétail :", trim($comment));
    
        return $comment;
    }


    //programme principal

    // Récupère les infos du fichier config pour la première page
    $config_file = fopen("config", "r") or die("Unable to open config file!");
    $client = substr(trim(str_replace("CLIENT=\"", "", fgets($config_file))),0,  -1);
    $produit = substr(trim(str_replace("PRODUIT=\"", "", fgets($config_file))), 0, -1);
    $version = trim(str_replace("VERSION=", "", fgets($config_file)));
    fclose($config_file);
    $date = date("d/m/Y");

    // Récupère les fichiers .c
    $files = scandir("./");
    $c_files = array();
    foreach ($files as $file) {
        if (substr($file, -2) == ".c") {
            // check si le fichier contient une entête. Si oui, on l'ajoute à la liste des fichiers à traiter
            $tmp = fopen($file, "r");
            $line = fgets($tmp);
            if (substr($line, 0,3) == "/**") {
                $c_files[] = $file;
            }
            fclose($tmp);
        }
    }
    $tmp = fopen("c_used", "a");
    foreach ($c_files as $file) {
        fwrite($tmp, $file . "\n");
    }
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DOCUMENTATION TECHNIQUE</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 20px auto;
            margin-top: 5em;
            padding: 10px;
            background-color: #f4f4f4;
        }
        h1 {
            margin-top: 8em;
            text-align: center;
        }
        header h2 {
            text-align: center;
            font-weight: bold;
        }
        pre {
            white-space: pre-wrap;
            margin: 1em 5em;
        }
        h2 {
            text-align: center;
            font-weight: bold;
            margin: 5em 0;
        }

        main h2 {
            margin: 3em 0;
        }
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
         }
         body {
            margin: 0;
            padding: 0;
         }
         
         section {
            border: solid 1px white;
            position: relative;
            height: 100vh;
            background-color: #f4f4f4;
         }

         strong {
            font-size: 1.2em;
            margin-bottom: 2em;
         }
         
         .bottom {
            position: absolute;
            bottom: 0;
            border: 1px dotted red;
            min-height: 5rem;
         }
         
         /* @page { size: A4 } */
         
         @media print {
            section {
               width: 8.26in;
               height: 11.66in;
               margin: 0;
               /* change the margins as you want them to be. */
               page-break-after: always;
            }
            div {
                margin: 1em 5em;
            }
         }
    </style>
</head>
<body>
    <!-- Première page -->
    <section>
        <header>
            <h1><?php echo $client ?></h1>
            <h2><?php echo $produit ?></h2>
            <h2><?php echo $version ?></h2>
            <h2><?php echo $date ?></h2>
        </header>
    </section>

    <main>
        <!-- Index des fichiers -->
        <section>
            <h2>Sommaire</h2>
            <ul>
                <?php
                    foreach ($c_files as $file) {
                ?>
                <li>
                    <a href="#<?php echo $file ?>">
                        <div style="border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; background-color: #fff; text-align: center;">
                            <pre><?php echo $file ?></pre>
                        </div>
                    </a>
                </li>
                <?php
                    }
                ?>
            </ul>
        </section>

        <!-- Traitements des fichiers un par un -->
        <?php
            foreach ($c_files as $c_file) {
                echo "\t<!-- Fichier " . $c_file . " -->\n";
                // check si le fichier contient une entête. Si non, on passe au suivant
        ?>
        <!-- Entete du programme -->
        <section id="<?php echo $c_file ?>">
            <h2><?php echo $c_file ?></h2>
            <div style="border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; background-color: #fff; text-align: center;">
                <pre><?php 
                // Utilisation de preg_match pour récupérer le texte du commentaire en C
                echo get_entete($c_file);
                ?></pre>
            </div>
        </section>
        
        <?php            
            // check si le fichier contient des #define. Si non, on passe au suivant
            $defines = get_define($c_file);
            if (count($defines[1]) == 0) {
                echo "\t<!-- Fichier " . $c_file . " ne possède pas de #define -->\n";
            } else {
        ?>        
        <!-- Defines -->
        <section>
            <h2>DEFINES</h2>
            <?php
                // $defines = get_define($c_file);
                foreach ($defines[1] as $key => $define) {
            ?>
            <div style="border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; background-color: #fff;">
                <pre><?php
                    echo trim($define);
                    echo "<br><em>" . $defines[3][$key] . "</em>";
                ?></pre>
            </div>
            <?php
                }
            ?>
        </section>
        <?php
            }
            
            // check si le fichier contient des typedef struct. Si non, on passe au suivant
            $matches = get_struct($c_file);
            if (count($matches[2]) == 0) {
                echo "\t<!-- Fichier " . $c_file . " ne possède pas de typedef struct -->\n";
            } else {
        ?>
        <!-- Structures -->
        <section>
            <h2>STRUCTURES</h2>
            <?php
                // $matches = get_struct($c_file);
                foreach ($matches[2] as $key => $struct) {
            ?>
            <div style="border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; background-color: #fff;">
                <pre><?php
                    echo trim($struct) . "\n";
                    echo "<em> " . $matches[3][$key] . "</em>\n\n";
                    echo "typedef struct {\n";
                    // fait apparaitre les attributs de la structure
                    foreach ($matches[1][$key] as $attribute) {
                        echo "\t" . $attribute[1][0] . "\t<em>" . $attribute[2][0] . "</em>\n";
                    }
                    echo "} " . $struct . ";\n";
                ?></pre>
            </div>
            <?php
                }
                ?>
        </section>
        <?php
            }
            
            // check si le fichier contient des variables globales. Si non, on passe au suivant
            $globales = get_globales($c_file);
            if (count($globales[3]) == 0) {
                echo "\t<!-- Fichier " . $c_file . " ne possède pas de variables globales -->\n";
            } else {
        ?>
        <!-- Globales -->
        <section>
            <h2>GLOBALES</h2>
            <?php
                // $globales = get_globales($c_file);
                foreach ($globales[3] as $key => $var) {
            ?>
                <div style="border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; background-color: #fff;">
                    <pre><?php
                        echo trim($var). "\n";
                        echo "<em> " . $globales[5][$key] . "</em>\n\n";
                        if ($globales[1][$key] != "") {
                            echo $globales[1][$key] . " " . $globales[2][$key] . " " . $var . " = " . $globales[4][$key] . ";\n";
                        } else {
                            echo $globales[2][$key] . " " . $var . " = " . $globales[4][$key] . ";\n";
                        }
                    ?></pre>
                </div>
            <?php
                }
            ?>
        </section>
        <?php
            }
            
            // check si le fichier contient des fonctions. Si non, on passe au suivant
            $matches = get_fonctions($c_file);
            if (count($matches[2]) == 0) {
                echo "\t<!-- Fichier " . $c_file . " ne possède pas de fonctions -->\n";
            } else {
        ?>
        <!-- Fonctions -->
        <section>
            <h2>FONCTIONS</h2>
            <?php
                $pattern = "/\s\/\*\*(\s*\*\s*.*\s)+((\w*)?\s*(\w*)\s*(\w*)\s*\((.*)\)\s*\{)/";
                preg_match_all($pattern, file_get_contents($c_file), $matches);
                foreach ($matches[2] as $key => $func) {
            ?>
                <div style="border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; background-color: #fff;">
                    <pre><?php
                        echo "<strong>" . trim(substr(trim($func), 0, -1)) . "</strong>\n\n";
                        echo process_func_comment($matches[0][$key]) . "\n\n";
                    ?></pre>
                </div>
            <?php
                }
            ?>
        </section>        
        <?php
                }
            }
        ?>
    </main>
</body>
</html>