# 63 — Retirer un plafond de journaux non prévu avant admission

## Contrôle du candidat

Le coordinateur a lu toute la nouvelle région native Windows, sa note d'admission par empreinte et les risques déclarés. Une limite de 16 Mio par sortie a été ajoutée : le code écrit le préfixe restant puis signale une panne. Cette limite est explicite, mais ne faisait pas partie du contrat ratifié de conservation des sorties brutes.

L'admission à l'exécution est suspendue sur ce point. Le propriétaire doit retirer le quota et la troncature, sans les remplacer par un autre plafond, en conservant les transferts par blocs et les erreurs IO explicites. Les tests et assertions restent inchangés. Le programme factice est déjà borné à de petites sorties ; aucun quota produit n'est nécessaire pour son confinement.

## Autre limite conservée

Le code ferme les lecteurs avant d'attendre les lectures asynchrones restantes. Le comportement face à un descendant conservant les handles n'est pas démontré ; une fermeture synchrone peut elle-même constituer une limite opérationnelle. Le faux enfant ne crée aucun descendant, et un futur GREEN des cas actuels ne devra pas être généralisé à cette situation.

Cette frontière devra être révisée avant raccordement du helper complet. Aucun nouvel essai de descendant, jonction, changement de droits ou outil moteur n'est autorisé. Le pin reste une admission après lecture, pas une sandbox ni une preuve de correction.

## Références et résultats

Source `71cf8b0f032794814c3697e1f2fe3720af7c4759`, pin/documentation `d6715a21cce6732f2a54bb4ebe010f6f6edee21e`. Localisation : `scripts/build-windows.ps1:421,429–432,495–505`, notes `NATIVE_GREEN.md` et `NATIVE_PIN_ADMISSION.md`.

Aucun test de ce candidat GREEN exécuté par le coordinateur. Le RED antérieur reste acquis dans sa portée ; aucun moteur, MSVC, compilation, rendu, benchmark, installation ou cloud lancé.
