# QA Linux — cause du Reader recoupée, solution portable encore ouverte

## Objectif et conclusion à cette étape

Le diagnostic des trois échecs graphiques Linux est livré et sa cause locale est recoupée par le lead. Le moteur distingue les attributs tableaux uniquement grâce au suffixe du nom réfléchi. Mesa renvoie un nom nu : les tests échouent sur leur recherche de clé avant de vérifier le refus d'upload attendu. Le risque dépasse donc une simple différence d'affichage, mais aucune mutation indue de buffer n'est encore reproduite.

Le diagnostic est accepté à sa portée, sans adopter de correctif. L'expérience scalaire/tableaux reste à exécuter par le propriétaire du bail GPU. Aucun alias, renommage public, skip ou nouvelle provenance de métadonnées n'est autorisé.

## Code observé et conséquences

Le lead a relu les portions ciblées du candidat `a4c0f82` :
- `AttributeReader.hpp:74–107` garde le nom natif comme clé et appelle `setGLArray(name.ends_with("[0]"))`. Avec un nom nu, size1 et type vec3, ce bit devient false.
- `attribute/Setter.hpp:67–82` refuse `isGLArray() || getGLSize()!=1` avant les mutations natives. Un tableau size2 reste refusé par sa taille ; pour size1 au nom nu, ce garde ne suffit plus à prouver le refus.
- Selon la remise spécialiste, les tests Reader et rendu cherchent directement `qaArray[0]` ; l'arrêt observé ne démontre donc pas encore une corruption du buffer leurre ou de l'image. Le préflight s'arrête aussi au premier cas et ne prouve pas son second tableau.

Localisation symbolique : Serena confirme la racine canonique ARTIST, mais `find_symbol` ne remonte ici que la déclaration primaire du template `OpenGLPassAttributeReader`. Les corps nécessaires ont été relus par portions de fichier. Cette limite de couverture n'est pas une preuve d'absence de consommateurs. Aucun contrat ou source de production n'a été modifié.

## Norme OpenGL3.3 : texte primaire maintenant extrait

La remise spécialiste indiquait ne pas avoir extrait proprement les pages PDF3.3. Le lead a pu le faire avec `pypdf` déjà installé ; aucune installation nécessaire. Le site registry a retourné HTTP403, sans nouvelle tentative de cet accès ; le PDF du dépôt public officiel Khronos a été récupéré et analysé.

Document : **OpenGL3.3 Core Profile, 11mars2010**, §2.11.3, page imprimée56 (pagePDF71). Extraits exacts :

> The returned attribute name must be the name of a generic attribute.

> The size of the attribute is returned into size. The value in size is in units of the type returned in type.

Pour GetAttribLocation :

> returns the generic attribute index that the attribute variable named name was bound to when the program object named program was last linked.

> If name is not an active attribute, or if an error occurs, -1 will be returned.

Ce passage ne précise pas l'orthographe `[0]` ni les règles d'alias base/élément. **Cela ne démontre pas une absence de règle dans toute la spécification**, et n'autorise pas à supposer une discrimination universelle des scalaires par une requête `[0]` en3.3.

La remise spécialiste cite des règles explicites de noms et d'équivalence legacy/moderne dans ARB_program_interface_query et OpenGL4.3+. Leur portée ne doit pas être transposée sans gate à3.3. Elle cite aussi le code d'un miroir Mesa tag25.2.8 suggérant une différence entre les routes legacy et moderne ; ce n'est pas une preuve de la construction Ubuntu exécutée ni un verdict de conformité du pilote.

## Expérience autorisée, encore NOT_RUN dans ce rapport

Trois programmes GLSL330 actifs, scalaire/array1/array2 ; compilation et link doivent d'abord réussir. Comparer réflexion legacy, localisations base et éléments, nom inexistant et erreursGL. Avec version4.3 ou extension effectivement disponible et fonctions chargées, comparer aussi la réflexion moderne PROGRAM_INPUT ; aucun appel moderne non supporté.

L'expérience est réservée au producteur déjà propriétaire du bail Linux, sous forme de source/exécutable temporaires autonomes et sorties privées. Aucun besoin de draw/upload ni de mutation d'un test de production. Les trois échecs de la suite restent conservés. Une distinction observée sur Mesa4.5 ne devient pas une garantie du socle3.3.

## Acceptation, limites et suite

La tâche de diagnostic lecture seule est acceptée pour la cause de code, les limites et l'expérience proposée ; elle ne livre pas de correction ni de nouvelle preuve GPU. Le spécialiste a remis son bilan final sans toucher aux sources ou au bail d'exécution. Un complément documentaire envoyé après sa fin a été stocké sans le réveiller ; le présent supplément primaire est attribué au lead, pas au spécialiste.

Avant implémentation : résultat de sonde, contrat de clés et preuve de classification à borner. Si la solution exige des alias publics, une nouvelle provenance source/cook ou une autre frontière durable, consultation architecte puis décision appropriée avant auteur. Pas de demande utilisateur routinière ajoutée tant que l'enquête bornée peut continuer.

La parité Linux reste FAIL au résultat normal actuel. La campagne ASan/CI indépendante continue ; aucun PASS global, performance, fuite exhaustive ou nouveau backend revendiqué. Statuts ADR inchangés.

## Annexe technique et provenance

- Diagnostic : `task_mvseouf3_0012_cb89ee`, pool-6/gpu-engineer, lecture seule ; acceptation lead sur remise native `team_results`.
- Norme primaire : `https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/specs/gl/glspec33.core.pdf`.
- PDF : 2620959octets, 425pages, SHA256 `dc1d454f0a889f9f2b6e6f645e1110dc24939396265f57eab35ae9d4e77cfd5c`.
- Copies : `out/logs/mission-linux-qa/glspec33.core.pdf` et `glspec33-core-printed-page-56.txt`.
- Commandes lead : extraction Python3.13/pypdf via Context Mode ; navigation Serena et lectures ciblées. Aucun configure/build/testGPU exécuté pour ce rapport.
- Rapport13 : observation JUnit Linux75cas/3failure/0skip et plan de sonde ; rapport12 : preuve Windows au candidat indiqué.
