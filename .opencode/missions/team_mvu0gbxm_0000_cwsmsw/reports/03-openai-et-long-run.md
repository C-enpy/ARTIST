# 3 — OpenAI uniquement et poursuite autonome de la mission

## Correction demandee

Livrer le pipeline Windows/Linux, avec uniquement les modeles du fournisseur OpenAI et le mode long run. Ce mode permet de poursuivre les etapes autorisees sans nouvel accord a chaque jalon ; il ne leve aucune limite de validation ou de securite.

## Changements du brouillon

- providers limite a openai ; 11 couples modele/variante OpenAI conserves sur les 27 precedents. Les autres fournisseurs, y compris les routes opencode-go portant un nom de modele OpenAI, sont retires.
- Identifiants, variantes et metadonnees des entrees conservees inchanges ; seuil initial de planification maintenu a 50.
- Ajout du contrat natif autonomy.long_run : poursuivre la livraison dans le perimetre convenu, validations du pipeline seul, aucune campagne moteur/rendu/benchmark et aucune ressource cloud.
- QA proportionnee et six criteres de reception inchanges ; aucune classification ajoutee ni configuration globale modifiee.

Le plan et l'arbitrage sur les validations restent ceux des rapports 01 et 02. Les tests synthetiques du pipeline sont permis seulement apres l'approbation native. Les qualifications graphiques et performances Windows/Linux demeurent non executees.

## Etat et suite

Brouillon non approuve ; aucun worker d'execution demarre. La nouvelle revision doit etre presentee par /ensemble-mission config avant que l'utilisateur puisse l'approuver par /ensemble-mission start. Le precedent hash de proposition ne couvre pas cette revision.
