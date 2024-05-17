<?php
// Lit et stock dans un tableau le contenu du fichier "DOC_UTILISATEUR"
$fichier = file('./doc.md');
// Variable qui contient le regex du tableau
$patternTab = '/\|([^|]+)\|([^|]+)\|([^|]+)\|/';
// Variable pour stocker la ligne précédente
$lastLine = '';
$og_line = '';

// Récupère les infos du fichier config pour la première page
$config_file = fopen("config", "r") or die("Unable to open config file!");
$client = substr(trim(str_replace("CLIENT=\"", "", fgets($config_file))),0,  -1);
$produit = substr(trim(str_replace("PRODUIT=\"", "", fgets($config_file))), 0, -1);
$version = trim(str_replace("VERSION=", "", fgets($config_file)));
fclose($config_file);
$date = date("d/m/Y");

// premiere page
$premierepage = "<!DOCTYPE html>
<html lang=\"en\">
<head>
    <meta charset=\"utf-8\">
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
    <title>DOCUMENTATION TECHNIQUE</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 20px 1em;
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
        h2, h3, h4, h5, h6 {
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
            <h1>$client</h1>
            <h2>$produit</h2>
            <h2>$version</h2>
            <h2>$date</h2>
        </header>
    </section>";

echo $premierepage;

// Définition de la fonction qui permet de compter les récurrences des '#'
// Définition de la fonction qui permet de compter les récurrences des '#'
function niveau_titre($matches) {
  $niveau_titre = strlen($matches[1]);
  return "<h$niveau_titre>{$matches[2]}</h$niveau_titre>"; 
}

// Affiche toutes les lignes du tableau
foreach ($fichier as &$ligne) {   
/*********  TITRE  **********/
$ligne = preg_replace_callback('/(^#{1,4})\s(.*)/','niveau_titre', $ligne);

/*********  LISTE  **********/
if (preg_match('/^-/', $ligne)&&($listeStart == '')) {
  $ligne = "<ul>" . preg_replace('/^-(.*)/', '$1', $ligne);
  $listeStart = 1;
} elseif (!preg_match('/ -(.*)/', $ligne)&&($listeStart == 1)) {
  $listeStart = 0;
  $ligne = "</ul>\n";
}
if (preg_match('/ -(.*)/', $ligne)){
  $ligne =  "\t<li>\n" . preg_replace('/ -(.*)/', "\t\t".'$1', $ligne) . "\t</li>\n";
}


/*********  TABLEAU  **********/
preg_match_all($patternTab, $ligne, $matches, PREG_SET_ORDER);
if ($matches){

  // Check si c'est la 1ere ligne du tableau
  // Si 1ere ligne alors on ouvre le tableau et on écrit les en-têtes
  if (preg_match($patternTab, $ligne)&&($tabStart == '')) {
    $tabStart = 1;
    $ligne = "<table>\n";
    $ligne .= "\t<tr>\n";
    for ($i = 1; $i <= count($matches[0]) - 1; $i++) {
      $ligne .= "\t\t<th>" . trim($matches[0][$i]) . "</th>\n";
    }
    $ligne .= "\t</tr>\n";
  } elseif (preg_match('/\|-\|-?/', $ligne)) {
    //Check si la ligne est l'indication de tableau en Markdown
    //Si oui alors on supprime cette ligne car inutile en html
    $ligne = '';
  } elseif (preg_match($patternTab, $ligne)) {
    //Check si la ligne si la ligne actuelle est un tableau
    //Si oui alors on l'ajoute au tableau
    $ligne .= "\t<tr>\n";
    for ($i = 1; $i <= count($matches[0]) - 1; $i++) {
      $ligne .= "\t\t<td>" . trim($matches[0][$i]) . "</td>\n";
    }
    $ligne .= "\t</tr>\n";
    // Sert à supprimer la ligne markdown qui restait afficher
    $ligne = preg_replace($patternTab, '', $ligne);
  }
  // Check si c'est la fin du tableau
} elseif ($tabStart == 1) {
  // Terminez le tableau HTML
  $tabStart = 0;
  $ligne = "</table>\n"; 
}

/*********  CODE  **********/
if (preg_match('/```/',$ligne)&&($codeStart != 1)) {
  // Si la ligne est un code on l'enregistre
  $codeStart = 1;
  $ligne = "<pre>\n";
}
if (($codeStart == 1)&&($tour == 1)){
  // Si la ligne est après la balise de code, on l'enregistre
  $ligne = preg_match('/.*/',$ligne);
}
if (preg_match('/```/',$ligne)&&($codeStart == 1)) {
  // Si la ligne est un code on l'enregistre
  $ligne = "</pre>\n";
  $codeStart = 0;
}

/*********  LIEN  **********/
$ligne = preg_replace('/\[(.+?)\]\((.+?)\)/', '<a href="$2">$1</a>', $ligne);

/*********  TEXTE EN GRAS / ITALIQUE  **********/
$ligne = preg_replace('/<b\>(.*)<\/b>/', '<strong>$1</strong>', $ligne);
$ligne = preg_replace('/<i\>(.*)<\/i>/', '<em>$1</em>', $ligne);

/*********  AFFICHAGE  **********/
echo $ligne;
}
?>