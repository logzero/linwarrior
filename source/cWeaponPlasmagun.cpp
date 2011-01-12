#include "cWeaponPlasmagun.h"

#include "psi3d/snippetsgl.h"

#include <cassert>


cWeaponPlasmagun::cWeaponPlasmagun() {
    clipSize = 19;
    depotSize = 9;
    remainingAmmo = clipSize;
    remainingClips = depotSize;

    if (WEAPONSOUND) {
        //ALuint buffer = alutCreateBufferHelloWorld();
        ALuint buffer = alutCreateBufferFromFile("data/freesound/plasmagun.wav");
        alGenSources(1, &soundSource);
        alSourcei(soundSource, AL_BUFFER, buffer);
        alSourcef(soundSource, AL_PITCH, 1.0f);
        alSourcef(soundSource, AL_GAIN, 1.0f);
        alSourcei(soundSource, AL_LOOPING, AL_FALSE);
    }
}

void cWeaponPlasmagun::fire(OID target) {
    if (!ready()) return;

    if (remainingAmmo > 0) {
        remainingAmmo--;
        if (remainingAmmo == 0 && remainingClips != 0) {
            timeReloading = 2.5;
        } else {
            timeReloading = 0.3;
        }
    }

    float* source = weaponPosef;

    cParticle* s = new cParticle();
    assert(s != NULL);
    s->fuel = 1.0f;

    float* pos = &source[12];
    vector_cpy(s->pos, pos)

            float nrm[3];
    float pos2[] = {0, 0, -1};
    matrix_apply2(source, pos2);
    vector_sub(nrm, pos2, s->pos);

    vector_scale(s->vel, nrm, 50);
    shrapnelParticles.push_back(s);

    playSourceIfNotPlaying();
}

void cWeaponPlasmagun::animate(float spf) {

    foreachNoInc(i, shrapnelParticles) {
        cParticle* s = *i++;
        s->pos[0] += s->vel[0] * spf;
        s->pos[1] += s->vel[1] * spf;
        s->pos[2] += s->vel[2] * spf;
        s->fuel -= spf;
        if (s->fuel < 0) {
            shrapnelParticles.remove(s);
            delete s;
        } else {
            float radius = 0.2;
            int roles = 0;
            float damage = 13;
            int damaged = this->damageByParticle(s->pos.data(), radius, roles, damage);
            if (damaged) {
                shrapnelParticles.remove(s);
                delete s;
            }
        }
    }

    timeReloading -= spf;
    if (timeReloading < 0) {
        timeReloading = 0.0f;
        if (remainingAmmo == 0 && remainingClips != 0) {
            remainingAmmo = clipSize;
            if (remainingClips > 0) remainingClips--;
        }
    }
}

void cWeaponPlasmagun::drawSolid() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        glEnable(GL_NORMALIZE);
        glDisable(GL_CULL_FACE);
        glPushMatrix();
        {
            glMultMatrixf(weaponPosef);
            glRotatef(-90, 1, 0, 0);

            // Scale
            float scale = weaponScale;
            glScalef(scale, scale, scale);

            if (this->ready() == 0 && remainingAmmo != 0) {
                glDisable(GL_LIGHTING);
                glColor4f(0.2, 0.2, 0.3, 1);
                glPushMatrix();
                {
                    glScalef(0.02, 0.02, 0.02);
                    glTranslatef(0, (0.5 + 0.5 * sin(this->weaponOwner->seconds * 2 * M_PI))* 1.7 / 0.02, 0);
                    glRotatef(this->weaponOwner->seconds * 2 * 360, 1, 3, 7);
                    cPrimitives::glUnitBlock();
                }
                glPopMatrix();
                glColor4f(0.2, 0.3, 0.2, 1);
                glPushMatrix();
                {
                    glScalef(0.02, 0.02, 0.02);
                    glTranslatef(0, (0.5 + 0.5 * cos(this->weaponOwner->seconds * 2 * M_PI))* 1.7 / 0.02, 0);
                    glRotatef(this->weaponOwner->seconds * 2 * 360, 1, 3, 7);
                    cPrimitives::glUnitBlock();
                }
                glPopMatrix();
                glColor4f(0.3, 0.2, 0.2, 1);
                glPushMatrix();
                {
                    glScalef(0.02, 0.02, 0.02);
                    glTranslatef(0, (0.5 - 0.5 * sin(this->weaponOwner->seconds * 2 * M_PI))* 1.7 / 0.02, 0);
                    glRotatef(this->weaponOwner->seconds * 2 * 360, 1, 3, 7);
                    cPrimitives::glUnitBlock();
                }
                glPopMatrix();
            }

            glEnable(GL_LIGHTING);
            glColor4f(0.2, 0.2, 0.3, 1.0);
            glPushMatrix();
            {
                glScalef(0.1, 0.14, 0.12);
                cPrimitives::glCenterUnitBlock();
            }
            glPopMatrix();

            glColor4f(0.1, 0.1, 0.1, 1.0);
            glPushMatrix();
            {
                glTranslatef(0.0, 0.8, 0);
                glScalef(0.03, 0.8, 0.03);
                glTranslatef(2.0, 0, 0);
                cPrimitives::glCenterUnitCylinder(7);
                glTranslatef(-4.0, 0, 0);
                cPrimitives::glCenterUnitCylinder(7);
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void cWeaponPlasmagun::drawEffect() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_ALL_ATTRIB_BITS);
    {

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        float n[16];
        SGL::glGetTransposeInverseRotationMatrix(n);

        foreachNoInc(i, shrapnelParticles) {
            cParticle* s = *i++;
            glPushMatrix();
            {
                glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
                glMultMatrixf(n);
                glColor4f(0.1, 0.1, 0.6, 0.99f);
                cPrimitives::glDisk(9, 1.9 * 0.1f);
                glColor4f(0.8, 0.8, 0.1, 0.6f);
                cPrimitives::glDisk(7, 1.9 * 0.07f);
            }
            glPopMatrix();
        }

    }
    glPopAttrib();
}

void cWeaponPlasmagun::drawHUD() {
    float a = 0.9;
    float b = 0.6;

    // Ammo
    float i = 0.1;
    float j = 0.9;
    float r = i + (remainingAmmo / (float) clipSize) * (j - i);
    glBegin(GL_QUADS);
    glColor4f(1, 1, 0, b);
    glVertex3f(i, j, 0.0);
    glVertex3f(i, i, 0.0);
    glColor4f(1, 0, 0, b);
    glVertex3f(r, i, 0.0);
    glVertex3f(r, j, 0.0);
    glEnd();

    // Clips
    float l = 0.1;
    float h = 0.15;
    r = i + (remainingClips / (float) depotSize) * (j - i);
    glBegin(GL_QUADS);
    glColor4f(0, 0, 0, a);
    glVertex3f(l, i + h, 0.0);
    glVertex3f(l, i, 0.0);
    glColor4f(1, 1, 1, a);
    glVertex3f(r, i, 0.0);
    glVertex3f(r, i + h, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor4f(0, 0, 1, b);
    glVertex3f(0.25, 0.5, 0.0);
    glColor4f(1, 1, 1, a);
    glVertex3f(0.75, 0.5, 0.0);
    glEnd();
}

