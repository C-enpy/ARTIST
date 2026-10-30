# SOURCE_FIX_READY — correction de l'ecart de capture, sans run

Base de revue : source `71cf8b0f032794814c3697e1f2fe3720af7c4759`,
admission candidate `d6715a21cce6732f2a54bb4ebe010f6f6edee21e`.
Le lead a relu ces candidats et **n'a accorde aucun run**. Aucun resultat GREEN
n'a ete observe ; la preuve RED reste celle sur `77900399fb9f18bc9f75d17904f8f8195b138321`.

## Premiere cause et correction autorisee

Ecart source constate : un quota cumule par flux, non ratifie, pouvait ecrire
une fraction du bloc lu puis echouer. Cela ne respecte pas la conservation des
journaux bruts complets ; les petits cas factices ne pouvaient pas qualifier ce
choix et aucun passage de test n'est revendique.

Correction unique : supprimer le quota, sa branche de troncature, le compteur
associe et les mentions le presentant comme contrat. Chaque bloc recu est
desormais ecrit en entier. Aucun autre plafond cumule ni relabel de quota.
Buffers/chunks de 64 Kio inchanges ; ce bornage de memoire n'est pas un quota
sur les journaux. Les exceptions IO, stop et drain restent des pannes explicites.
Les logs deja ecrits restent disponibles si une erreur IO empeche de continuer ;
le candidat ne pretend pas garantir l'ecriture au-dela d'une erreur de stockage.

Le code source est commite separement du nouveau pin/admission, a relire
ensemble avant toute execution. NATIVE_PIN_ADMISSION.md donne les identites
exactes. Le pin demeure une porte de revue, pas une sandbox.

## Frontieres preservees

- Quatre entrees seulement ; aucune autre fonction, schema/core, CLI ou outil.
- 15 methodes natives et 37 anciennes, assertions, fixtures et enfant inchanges.
- test06 reparse NON_EXECUTE, pas d'autre jonction/symlink/privilege.
- Le faux enfant n'a aucun descendant ; aucune nouvelle experience de processus.
- Dispose du lecteur avant pending.Wait demeure une operation synchrone fragile
  face a un writer herite. Les delais de Wait n'en font pas un delai absolu et
  ne prouvent pas la reclamation generale. Aucun fix de ce risque n'est invente
  ou revendique dans la correction de quota.
- Capture de production : revue independante requise avant helper complet.
  Refus du helper complet maintenu ; le pin ne leve pas cette frontiere.

## Matrice reelle et commandes

- Auteur : comparaison Git, digest et parsing AST statiques seulement.
- Nouveau source, pin-gate, 14 tests selectionnes et anciens 37 : NON_EXECUTE.
- Runtime, reclamation lecteurs, arbres de processus et payload reparse :
  aucune preuve nouvelle. Aucun moteur/MSVC/vcvars/build/install/GL/cloud.

Les commandes exactes et leur cwd demeurent dans NATIVE_GREEN.md : 14 noms
explicites hors06, puis les 37 tests historiques. Elles sont proposees au lead
uniquement APRES nouvelle revue/admission, pas autorisees par cette note.
Attendu theorique inchange : 14/14 + 37/37 sans failure/error/skip. Cela n'est
ni un resultat observe ni une preuve de correction de la capture de production.
