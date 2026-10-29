# Publication du contexte réutilisable sur main

## Portée

Publication autorisée des changements locaux recensés : règles et profils QA,
validateurs, notes de provenance, rapports et politiques de missions, patch
historique et exports publics de session. Les politiques archivées ne constituent
ni un nouveau start, ni une nouvelle autorisation de modèle ou de permission.

Deux exclusions seulement dans cet inventaire : les fichiers de `.tmp/`, laissés
locaux comme demandé, et la configuration GrepAI active contenant l'identifiant
local. Celle-ci reste intacte sur disque mais n'est plus suivie par Git ; l'exemple
public contient un placeholder. Aucun moteur, shader, build ou workflow modifié.

## Vérifications exécutées

- Harnais QA : baseline 594 tests, dont 5 échecs reproduits ; raccordement au même
  contrat v2 déjà choisi pour les trois profils, sans modifier leurs permissions.
- RED ciblé de raccordement : 6 tests, 4 échecs attendus. GREEN : 633/633.
- Revue indépendante fresh puis correction d'un gap protégeant la consigne
  « communication seulement, pas runner » du planner : RED 1/1, puis **634/634**,
  zéro échec ou skip. Ré-audit indépendant reçu : finding fermé.
- Validateur existant : syntaxe 41 contrôles, schéma 81, hors ligne 215, tous PASS ;
  missionVerdict INCONCLUSIVE conservé, aucun enforcement natif inventé.
- Sept JSON restants parsés sans clé dupliquée ; les onze exports publics gardent
  leurs SHA256 et leurs 385315 octets. Les avertissements d'espaces finaux de ces
  archives et du patch sont conservés pour préserver les sources, non masqués
  comme succès global de formatage. Diffcheck hors archives propre.
- Sélection exacte des chemins et contrôle des blobs indexés : pas de `.tmp`,
  pas de clé active ; empreintes des fichiers exclus conservées. Un seul fichier
  subit la normalisation CRLF standard de Git, sans changer le fichier local.
- QMD artist-wiki actualisé de façon ciblée ; contenu déjà à jour, embeddings
  présents. Lecture de la note de session float préparé confirmée par le MCP.

## Commandes et preuves

Runner : lead, distinct des deux profils QA en lecture seule. Tests :
`node --test --test-reporter=tap` sur les dix contrats T04 à T13 explicitement
énumérés ; `validate-agentic.mjs --mode syntax|schema|offline --root <ARTIST>
--manifest .opencode/tests/agentic/t13-integration/manifest.json`.

Plans et revue : `.opencode/evidence/context-publication/qa-plan.md` et
`qa-review.md`. Logs, inventaire, contrôles de l'index et vérification de publication
Git : `out/logs/context-publication/`. Secrets jamais imprimés dans les rapports.

Publication par commit(s) locaux sur main, fetch et push normal uniquement,
sans force, reset, clean, suppression des temporaires ou réécriture de l'historique.
Le dernier commit documentaire porte skip-ci : aucune source moteur/build ne
change ; les contrôles pertinents du harnais sont ceux réellement exécutés ci-dessus,
pas une nouvelle campagne GPU lancée pour publier des archives.

## Limites

Le scan de secrets porte sur les identifiants et motifs recherchés, pas une
garantie exhaustive. Le linter optionnel absent de l'installation reste NOT_RUN ;
les validateurs présents ont été exécutés. Aucun test moteur, nouveau canari,
rechargement natif de profil, enforcement, performance ou preuve GPU supplémentaire
n'est revendiqué par cette publication.
