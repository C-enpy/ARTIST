# Contrat adopté — accès uniforme float OpenGL préparé

Date : 2026-10-27. Adoption explicite par l'utilisateur des deux recommandations
de la conversation : mécanisme local et consommateur component-test avec pixels.
Cette adoption n'est ni un start natif ni une preuve d'implémentation.

## But et limites

Préparer une fois un uniform GLSL float strictement scalaire d'un programme
existant ; l'écrire plusieurs fois via un accès typé, sans résolution répétée
du nom/type. API par nom conservée. Pas de nouveau backend, cooker, render graph,
famille d'attributs, concurrence, gestionnaire global ou suppression générale de
std::any. Expériences de lecture de fichiers et réparation du harness exclues.

## État de départ

HEAD source observé : 2c8e9f235e82283a0686f2140e2a635db1ca2205. Chemins moteur,
tests, shaders et entrées de build examinés inchangés depuis a96104c. Les 555
normaux et 555 ASan Windows Debug de ce dernier sont historiques, pas des tests
du futur accès. Diffs préexistants harness/profils/.grepai FOREIGN à préserver.

ADR-0004 reste globalement Proposed/not-started dans les fichiers à cette date ;
seule cette tranche est adoptée. ADR-0003/0005 Accepted/partial ; ADR-0002 Proposed
avec tranche ClosedExecution livrée ; ADR-0008 vérifié Windows Debug seulement.
Ne promouvoir aucun statut d'implémentation avant les preuves correspondantes.

## API et moments de validation

Noms indicatifs : prepareFloatUniform(pass, name) -> PreparedFloatUniform,
puis binding.set(float). API OpenGL spécifique, sans nouvelle méthode virtuelle
obligatoire pour toutes les API. Construction publique depuis GLuint/location
interdite. Type d'accès et glUniform1f déterminés en C++ ; noms et faits de
réflexion validés à la préparation runtime après link, pas au cook offline.

Préparation : programme lié vivant, contexte autorisé courant, réflexion publiée
correspondant à son incarnation actuelle ; nom exact non vide sans NUL embarqué,
entrée issue de cette réflexion et non fabriquée par métadonnées manuelles,
GL_FLOAT exact, cardinalité 1 et location GLint valide. Rejeter aussi les arrays
de taille 1 à partir des informations de réflexion/canonicalisation disponibles,
sans assimiler GLSize==1 à la preuve d'une déclaration scalaire. Si cette
distinction n'est pas démontrable, refuser le cas ambigu, ne pas élargir le contrat.

Nom absent ou éliminé par optimisation : erreur requise déterministe d'uniform
actif introuvable ; pas d'accès silencieux vers -1 ni variante facultative.
Le chemin historique conserve ses propres compatibilités, dont GL_BOOL si elles
existent ; le nouveau chemin strict ne les modifie pas rétroactivement.

## Propriété et rejet avant danger

Owners actuels uniques du programme et des uniforms conservés. Accès emprunté,
sans prolongation des owners CPU ni du programme GPU. Témoins locaux révocables
du contexte de pass/programme/publication et du contexte d'uniform/métadonnées.
Leurs petits états peuvent utiliser shared_ptr/weak_ptr ; pas les ressources.
L'accès verrouille et vérifie témoins/générations AVANT toute déréférence du
contexte emprunté. Un compteur lu depuis un Pass* mort est interdit.

Contrat mono-thread, sérialisé et non réentrant. Un témoin n'est ni un verrou,
ni une fence GPU, ni un owner de GLFWwindow. Contexte GLFW vivant jusqu'au retrait
des ressources ; destruction/recréation pendant leur utilisation interdite.
Relink/delete GL externes contournant l'invalidation interdits pour ces owners,
ou notification préalable explicite si le contrat local l'exige.

## Matrice d'invalidation

- addUniform réussi : invalider conservativement tous les accès préparés de map
  avant remplacement éventuel ; prévalidation rejetée sans mutation les conserve.
- replaceUniforms réussi, map vide incluse : nouvelle publication ; tous les
  anciens accès expirent, même nom conservé. Rejet avant publication : aucun
  changement des générations, pointees et accès valides du même programme vivant.
- setUniformID/setProgramID/setGLType/setSize : invalider les accès concernés
  avant modification. Valeur seule/setValue : pas d'invalidation d'identité.
- Free/retrait programme : révocation avant premier effet natif de retrait.
  Échec après effets partiels : aucune réhabilitation des anciens accès.
- Publication d'un programme, même ID numérique : nouvelle incarnation ; ancienne
  réflexion non utilisable. Pas de retour silencieux à une génération ancienne
  lors d'un débordement : renouveler l'identité ou refuser.
- Destruction CPU : révocation avant disparition des objets empruntés.
- Moves de wrappers : les accès suivent le contexte transféré par unique_ptr ;
  les accès de l'ancienne destination expirent. Conserver la garantie existante
  si le cleanup est rejeté. Pas de copie implicite d'identité des contextes.

## Publication et écriture

Réflexion : candidat entièrement construit/validé avant publication ; map et
estampille publiées sans étape susceptible d'échouer entre les deux, puis ancienne
publication retirée. Échec de relecture du même programme vivant : ancienne map
valide conservée. Échec après free/reload : survie CPU possible, accès ancien
toujours périmé. Ne pas prétendre rendre load globalement transactionnel.

set préparé : témoins et générations, puis contexte autorisé, puis programme
réellement courant, puis miroir CPU et glUniform1f(location, argument float).
Pas de lookup de nom, réflexion, résolution de type ou any_cast sur ce chemin.
Conserver setValue<GLfloat> dans le std::any historique pour la cohérence des
lectures CPU ; transmettre directement l'argument typé au backend. Aucune promesse
de zéro allocation. Rejet des préconditions sans mutation CPU/GL ni upload.

## Consommateur et preuves

Migrer target dans ReflectedProgramGuardCrossTests.cpp ; garder decoy par nom.
L'utilisateur accepte ce consommateur réel du backend dans un component-test,
sans le présenter comme une application produit. Ajouter un exemple d'utilisation.

Oracle pixel : petite fixture utilisant le programme existant f2Scalar et un
triangle déplacé par ce float. Framebuffer linéaire, états fixes, initialisation
explicite des autres uniforms, pixels intérieurs/fond éloignés des arêtes. Oracle
indépendant non vide, égalité ancien/nouveau et contrôle par valeur différente.
Ce protocole doit être chiffré et fixé dans le plan QA avant code auteur.

Tests avant implémentation : préparation valide/absent/optimisé/type/arrays,
périmé avant danger, mutations de métadonnées, maps, free/reload, même ID simulé,
moves, mauvais contexte/programme, échecs de publication et miroir CPU.
Distinguer recherches de map CPU et résolution native ; ne pas compter seulement
glGetUniformLocation pour prétendre zéro lookup. Pas de gain annoncé sans mesure.

## Autorisation

Le start natif demandé par l'utilisateur reste nécessaire. Avant auteurs :
admission QA qualifiée, plan QA réconcilié et commité, contrat disponible dans
leurs worktrees. Intégration avant consommateurs et QA finale. Tout changement
matériel du présent contrat exige décision explicite, pas extension silencieuse.
