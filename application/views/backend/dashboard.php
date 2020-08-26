<section class="content">

    <div class="row">
        <div class="col-xl-8 col-md-8 mb-4">
            <div class="card">
                <div class="card-body">
                    <div class="table-responsive">
                        <table id="example" class="table table-bordered table-hover">
                            <thead class="bg-light text-dark">
                                <tr>
                                    <th>#</th>
                                    <th>ID Finger</th>
                                    <th>Nama</th>
                                    <th>TMT</th>
                                    <th>Action</th>
                                </tr>
                            </thead>
                            <tbody>
                                <?php $i = 1;
                                foreach ($data as $hasil) : ?>
                                    <tr>
                                        <td><?= $i++ ?></th>
                                        <td><?= $hasil['id_finger']; ?></td>
                                        <td><?= $hasil['nama']; ?></td>
                                        <td><?= date('d F Y - H:i:s', $hasil['created']); ?></td>
                                        <td>
                                            <a href="#" class="badge badge-warning delete-people" data-toggle="modal" data-target="#modalEdit<?= $hasil['id']; ?>"><i class="fa fa-edit"></i> Edit</a>
                                            <a href="<?= base_url() ?>Dashboard/hapusUser/<?= $hasil['id']; ?>" class="badge badge-danger delete-people tombol-hapus"><i class="fa fa-trash"></i> Hapus</a>
                                        </td>
                                    </tr>
                                <?php endforeach; ?>
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>
        </div>

        <div class="col-xl-4 col-md-4 mb-4">
          <form action="<?= base_url('Dashboard/editAkses'); ?>" method="post">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title" id="exampleModalLabel">Edit Akses</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="form-group">
                        <label for="kartu">Status</label>
                        <select class="custom-select" id="inputGroupSelect02" name="akses">
                            <option value="">-- Status --</option>
                            <option value="Daftar" <?php if ($akses[0]['akses'] == "Daftar") echo 'selected="selected"'; ?>>Daftar</option>
                            <option value="Masuk" <?php if ($akses[0]['akses'] == "Masuk") echo 'selected="selected"'; ?>>Masuk</option>
                        </select>
                    </div>
                </div>
                <div class="modal-footer">
                    <button type="submit" name="add" class="btn btn-primary">Edit</button>
                </div>
            </div>
        </form>
        </div>
    </div>

</section>

<!-- Modal Edit-->
<?php foreach ($data as $dt) : ?>
    <div class="modal fade" id="modalEdit<?= $dt['id']; ?>" tabindex="-1" role="dialog" aria-labelledby="exampleModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <form action="<?= base_url('Dashboard/editUser'); ?>" method="post">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title" id="exampleModalLabel">Edit User</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span>
                        </button>
                    </div>
                    <div class="modal-body">
                        <input type="hidden" value="<?= $dt['id']; ?>" name="id">
                        <div class="form-group">
                            <label for="nama">Nama User</label>
                            <input type="text" class="form-control" id="nama" name="nama" required autocomplete="off" value="<?= $dt['nama']; ?>">
                        </div>
                    </div>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                        <button type="submit" name="edit" class="btn btn-warning">Edit</button>
                    </div>
                </div>
            </form>
        </div>
    </div>
<?php endforeach; ?>