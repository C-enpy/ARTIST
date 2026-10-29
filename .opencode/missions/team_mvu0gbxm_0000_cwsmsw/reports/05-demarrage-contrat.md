# 5 — Demarrage du contrat du pipeline

## Objectif et etape

La revision 1 est approuvee : livrer le pipeline de verification pour qualification ulterieure Windows/Linux. La ratification du contrat a commence sur le profil engine-architect ; les huit consommateurs restent differes. Aucune nouvelle approbation utilisateur n'est requise pour les jalons ordinaires.

Les quatre branches de developpement seront paralleles seulement apres integration et versionnement du contrat et du plan QA. Les tests executables restent limites aux donnees synthetiques et programmes factices. Aucun moteur, rendu, benchmark, build moteur ou cloud n'est lance.

## Etat source et protection du travail existant

HEAD confirme : fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6. Diff preexistant sur les profils .opencode/agents, rapports d'une autre mission et .tmp : exclu des modifications et commits de cette mission. Les seuls nouveaux documents jusqu'ici sont le brouillon et les rapports de preparation de cette mission.

Le superviseur a transmis au proprietaire du contrat ce HEAD, les exclusions et les statuts ADR. La dependance de mesure reste un choix a ratifier, pas une installation implicite. Les schemas doivent distinguer etat de campagne, verdict des tests et validite des mesures.

## Preuves et limites

Observations natives : politique approuvee v1 ; contrat en cours sur pool-1, profil engine-architect ; 9 taches, aucune classification appelee. Observation Git : HEAD et status seulement, aucun test execute.

Avant admission QA, il faudra qualifier profil effectif, lecture et restitution. Les permissions elargies dans le diff utilisateur ne prouvent pas leur application native et ne seront pas modifiees silencieusement. Une revue de profil sur disque ne sera pas annoncee comme preuve d'isolation.
